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

//#include "autoconfig.c"
#include "services/autoc4/conf.c"

//#include "protocols/uip/uip.h"
#include "services/dmx-storage/dmx_storage.h"


static void autoc4_connack_callback(void);
static void autoc4_poll(void);
static void autoc4_publish_callback(char const *topic, uint16_t topic_length,
    const void *payload, uint16_t payload_length);


static volatile uint8_t *ddrs[] = IO_DDR_ARRAY;
static volatile uint8_t *ports[] = IO_PORT_ARRAY;
static volatile uint8_t *pins[] = IO_PIN_ARRAY;


static autoc4_config_t *autoc4_config = &config;
static bool *pin_input_states;


static const uint8_t zero_one[2] = { 0, 1 };


static void autoc4_connack_callback(void)
{
  /*
  for (int i=0; i<autoc4_config->input_count; i++)
  {
    bool input = (bool) (*pins[autoc4_config->input_configs[i].port_index] & 1<<autoc4_config->input_configs[i].pin_index);
    if (input)
      mqtt_construct_publish_packet(autoc4_config->input_configs[i].topic, zero_one + (autoc4_config->input_configs[i].inverted ? 0 : 1), 1, true);
    else
      mqtt_construct_publish_packet(autoc4_config->input_configs[i].topic, zero_one + (autoc4_config->input_configs[i].inverted ? 1 : 0), 1, true);
    pin_input_states[i] = input;
  }
  */

  // same effect, reduced code size?
  for (int i=0; i<autoc4_config->input_count; i++)
    pin_input_states[i] = 0;
  autoc4_poll();
  for (int i=0; i<autoc4_config->input_count; i++)
    pin_input_states[i] = 1;
  autoc4_poll();
}

static void autoc4_poll(void)
{
  for (int i=0; i<autoc4_config->input_count; i++)
  {
    bool input = (bool) (*pins[autoc4_config->input_configs[i].port_index] & 1<<autoc4_config->input_configs[i].pin_index);
    if (input && !pin_input_states[i])
      mqtt_construct_publish_packet(autoc4_config->input_configs[i].topic, zero_one + (autoc4_config->input_configs[i].inverted ? 0 : 1), 1, true);
    else if (!input && pin_input_states[i])
      mqtt_construct_publish_packet(autoc4_config->input_configs[i].topic, zero_one + (autoc4_config->input_configs[i].inverted ? 1 : 0), 1, true);
    pin_input_states[i] = input;
  }
}

static void autoc4_publish_callback(char const *topic,
    uint16_t topic_length, const void *payload, uint16_t payload_length)
{
  if (payload_length > 0)
  {
    // Set digital outputs
    for (int i=0; i<autoc4_config->output_count; i++)
      if (strncmp(topic, autoc4_config->output_configs[i].topic, topic_length) == 0)
      {
        if (((uint8_t*)payload)[0])
          *ports[autoc4_config->output_configs[i].port_index] |= 1<<autoc4_config->output_configs[i].pin_index;
        else
          *ports[autoc4_config->output_configs[i].port_index] &= ~(1<<autoc4_config->output_configs[i].pin_index);
        return;
      }

    // Set raw DMX channels
    if (strncmp(topic, autoc4_config->dmx_topic, topic_length) == 0)
    {
      set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, 0, payload_length);
      return;
    }

    // Set individual DMX devices
    for (int i=0; i<autoc4_config->dmx_count; i++)
      if (strncmp(topic, autoc4_config->dmx_configs[i].topic, topic_length) == 0)
      {
        if (payload_length > autoc4_config->dmx_configs[i].channel_count)
          payload_length = autoc4_config->dmx_configs[i].channel_count;
        set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, autoc4_config->dmx_configs[i].start_channel - 1, payload_length);
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


void
autoc4_init(void)
{
  mqtt_register_callback(&(mqtt_callback_config_t) {
      .connack_callback = autoc4_connack_callback,
      .poll_callback = autoc4_poll,
      .close_callback = NULL,
      .publish_callback = autoc4_publish_callback,
    });

  pin_input_states = malloc(autoc4_config->input_count);

  autoc4_ddr_init();
  mqtt_set_connection_config(autoc4_config->mqtt_con_config);
}

/*
  -- Ethersex META --
  header(services/autoc4/autoc4.h)
  init(autoc4_init)
*/
