/*
 * Copyright (c) 2014 by Philip Matura <ike@tura-home.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <string.h>

#include "protocols/mqtt/mqtt.h"
#include "autoc4.h"
#include "core/portio/portio.h"

#include AUTOC4_CONFIG_FILE

//#include "protocols/uip/uip.h"
#include "services/dmx-storage/dmx_storage.h"


static void autoc4_connack_callback(void);
static void autoc4_poll(void);
static void autoc4_publish_callback(char const *topic, uint16_t topic_length,
    const void *payload, uint16_t payload_length);

static void autoc4_read_inputs(void);
static void autoc4_init_input_states(void);
static void autoc4_set_output(uint8_t index, uint8_t state);
static uint8_t autoc4_get_output(uint8_t index);


static volatile uint8_t *ddrs[] = IO_DDR_ARRAY;
static volatile uint8_t *ports[] = IO_PORT_ARRAY;
static volatile uint8_t *pins[] = IO_PIN_ARRAY;


static bool logicer_state;

static autoc4_config_t *autoc4_config = &config;

typedef struct
{
  bool prev_state;
  bool mqtt_dirty; // whether there is a new state to be published
} autoc4_input_state_t;
static autoc4_input_state_t *pin_input_states;
typedef struct
{
  uint8_t value;
  uint8_t timer;
} autoc4_output_state_t;
static autoc4_output_state_t *output_states;


static const uint8_t blink_timeouts[4] = { 1, 5, 25, 30 }; // in 100 ms
static const uint8_t zero_one[2] = { 0, 1 };


static void autoc4_connack_callback(void)
{
  // mark every input as dirty, queue for mqtt resending
  for (int i=0; i<autoc4_config->input_count; i++)
    pin_input_states[i].mqtt_dirty = true;

  // send heartbeat message
  mqtt_construct_publish_packet_P(string_heartbeat_topic, zero_one + 1, 1, true);

  logicer_state = false;
}

static void autoc4_poll(void)
{
  for (int i=0; i<autoc4_config->input_count; i++)
    if (pin_input_states[i].mqtt_dirty)
    {
      const uint8_t *data;

      if (pin_input_states[i].prev_state)
        data = zero_one + (autoc4_config->input_configs[i].inverted ? 1 : 0);
      else
        data = zero_one + (autoc4_config->input_configs[i].inverted ? 0 : 1);

      if (mqtt_construct_publish_packet_P(autoc4_config->input_configs[i].topic, data, 1, true))
      {
        // publish successful
        pin_input_states[i].mqtt_dirty = false;
      }
      else
      {
        // the mqtt buffer might be full, cancel sending more publishes
        return;
      }
    }
}

static void autoc4_set_output(uint8_t index, uint8_t state)
{
  if (autoc4_config->output_configs[index].opendrain)
  {
    if (state)
      *ddrs[autoc4_config->output_configs[index].port_index] |= 1<<autoc4_config->output_configs[index].pin_index;
    else
      *ddrs[autoc4_config->output_configs[index].port_index] &= ~(1<<autoc4_config->output_configs[index].pin_index);
  }
  else
  {
    if (state)
      *ports[autoc4_config->output_configs[index].port_index] |= 1<<autoc4_config->output_configs[index].pin_index;
    else
      *ports[autoc4_config->output_configs[index].port_index] &= ~(1<<autoc4_config->output_configs[index].pin_index);
  }
}
static uint8_t autoc4_get_output(uint8_t index)
{
  return ((*ports[autoc4_config->output_configs[index].port_index] & (1<<autoc4_config->output_configs[index].pin_index)) != 0) // HIGH output
    ||    ((*ddrs[autoc4_config->output_configs[index].port_index] & (1<<autoc4_config->output_configs[index].pin_index)) == 0);     // open drain, high Z
}

static void autoc4_publish_callback(char const *topic,
    uint16_t topic_length, const void *payload, uint16_t payload_length)
{
  if (payload_length > 0)
  {
    // Set digital outputs
    for (int i=0; i<autoc4_config->output_count; i++)
      if (strncmp_P(topic, autoc4_config->output_configs[i].topic, topic_length) == 0)
      {
        // save value for blinking outputs
        output_states[i].value = ((uint8_t*)payload)[0];
        output_states[i].timer = 0;
        autoc4_set_output(i, ((uint8_t*)payload)[0]);
        return;
      }

    // Set raw DMX channels
    if (strncmp_P(topic, autoc4_config->dmx_topic, topic_length) == 0)
    {
      set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, 0, payload_length);
      return;
    }

    // Set individual DMX devices
    for (int i=0; i<autoc4_config->dmx_count; i++)
      if (strncmp_P(topic, autoc4_config->dmx_configs[i].topic, topic_length) == 0)
      {
        if (payload_length > autoc4_config->dmx_configs[i].channel_count)
          payload_length = autoc4_config->dmx_configs[i].channel_count;
        set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, autoc4_config->dmx_configs[i].start_channel - 1, payload_length);
        return;
      }

    // Save logicer heartbeat state
    if (strncmp_P(topic, string_logicer_heartbeat_topic, topic_length) == 0)
    {
      logicer_state = (bool) ((uint8_t*)payload)[0];
      return;
    }
  }
}

static void autoc4_ddr_init(void)
{
  // Enable outputs
  for (int i=0; i<autoc4_config->output_count; i++)
    *ddrs[autoc4_config->output_configs[i].port_index] |= 1<<autoc4_config->output_configs[i].pin_index;

  // Enable pullups
  for (int i=0; i<autoc4_config->input_count; i++)
    if (autoc4_config->input_configs[i].pullup)
      *ports[autoc4_config->input_configs[i].port_index] |= 1<<autoc4_config->input_configs[i].pin_index;
}


static void autoc4_poll_blinking(void)
{
  for (int i=0; i<autoc4_config->output_count; i++)
  {
    if (!autoc4_config->output_configs[i].enable_blinking)
      continue;

    if (output_states[i].value == 0)
    {
      autoc4_set_output(i, 0);
      continue;
    }

    if (output_states[i].timer == 0)
    {
      // check current output level
      if (autoc4_get_output(i))
      {
        // output HIGH

        // set timer
        output_states[i].timer = blink_timeouts[(output_states[i].value & 0xc0) >> 6] * (((output_states[i].value & 0x30)>>4) + 1);

        // special case, value == 0b0000 -> don't set output
        if (output_states[i].timer == 1)
          return;

        // toggle output
        autoc4_set_output(i, 0);
      }
      else
      {
        // output LOW

        // set timer
        output_states[i].timer = blink_timeouts[(output_states[i].value & 0x0c) >> 2] * (((output_states[i].value & 0x03)>>0) + 1);

        // special case, value == 0b0000 -> don't set output
        if (output_states[i].timer == 1)
          return;

        // toggle it
        autoc4_set_output(i, 1);
      }
    }

    output_states[i].timer--;
  }
}


static void autoc4_init_input_states(void)
{
  for (int i=0; i<autoc4_config->input_count; i++)
  {
    bool input = (bool) (*pins[autoc4_config->input_configs[i].port_index] & 1<<autoc4_config->input_configs[i].pin_index);
    pin_input_states[i].prev_state = input;
    pin_input_states[i].mqtt_dirty = true;
  }
}

static void autoc4_read_inputs(void)
{
  for (int i=0; i<autoc4_config->input_count; i++)
  {
    bool input = (bool) (*pins[autoc4_config->input_configs[i].port_index] & 1<<autoc4_config->input_configs[i].pin_index);
    if ((input && !pin_input_states[i].prev_state) || (!input && pin_input_states[i].prev_state)) // input has changed
    {
        pin_input_states[i].mqtt_dirty = true;
    }
    pin_input_states[i].prev_state = input;
  }
}


void
autoc4_periodic(void)
{
  static uint8_t counter = 10;
  if (--counter == 0)
  {
    // every 100ms
    counter = 10;
    autoc4_poll_blinking();
  }

  autoc4_read_inputs();
}

void
autoc4_init(void)
{
  mqtt_register_callback(&(mqtt_callback_config_t) {
      .connack_callback = autoc4_connack_callback,
      .poll_callback = autoc4_poll,
      .close_callback = NULL,
      .publish_callback = autoc4_publish_callback,
    });

  pin_input_states = malloc(autoc4_config->input_count * sizeof(autoc4_input_state_t));
  output_states = malloc(autoc4_config->output_count * sizeof(autoc4_output_state_t));

  logicer_state = false;

  autoc4_ddr_init();
  autoc4_init_input_states();
  mqtt_set_connection_config(autoc4_config->mqtt_con_config);
}

/*
  -- Ethersex META --
  header(services/autoc4/autoc4.h)
  init(autoc4_init)
  timer(1, autoc4_periodic())
*/
