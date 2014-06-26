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

#include "autoconfig.c"

//#include "protocols/uip/uip.h"
#include "services/dmx-storage/dmx_storage.h"


static void autoc4_connack_callback(void);
static void autoc4_poll(void);
static void autoc4_publish_callback(char const *topic, uint16_t topic_length,
    const void *payload, uint16_t payload_length);


static char const* const auto_subscribe_topics[] = { CONF_AUTOC4_MQTT_AUTOSUBS, NULL };

static const mqtt_connection_config_t mqtt_con_config = {
  .client_id = CONF_AUTOC4_MQTT_CLIENTID,
  .user = CONF_AUTOC4_MQTT_USER,
  .pass = CONF_AUTOC4_MQTT_PASS,
  .will_topic = NULL,
  .will_qos = 0,
  .will_retain = 0,
  .will_message = NULL,
  .target_ip = { CONF_AUTOC4_MQTT_IP },

  .connack_callback = autoc4_connack_callback,
  .poll_callback = autoc4_poll,
  .close_callback = NULL,
  .publish_callback = autoc4_publish_callback,

  .auto_subscribe_topics = auto_subscribe_topics,
};

static bool pin_input_states[INPUT_COUNT];


static const uint8_t zero_one[2] = { 0, 1 };


static void autoc4_connack_callback(void)
{
  /*
  for (int i=0; i<INPUT_COUNT; i++)
  {
    bool input = (bool) (*IO_PIN_ARRAY[input_configs[i].port_index] & 1<<input_configs[i].pin_index);
    if (input)
      construct_publish_packet(input_configs[i].topic, zero_one + (input_configs[i].inverted ? 0 : 1), 1, true);
    else
      construct_publish_packet(input_configs[i].topic, zero_one + (input_configs[i].inverted ? 1 : 0), 1, true);
    pin_input_states[i] = input;
  }
  */

  // same effect, reduced code size?
  for (int i=0; i<INPUT_COUNT; i++)
    pin_input_states[i] = 0;
  autoc4_autoconf_poll();
  for (int i=0; i<INPUT_COUNT; i++)
    pin_input_states[i] = 1;
  autoc4_autoconf_poll();
}

static void autoc4_poll(void)
{
  for (int i=0; i<INPUT_COUNT; i++)
  {
    bool input = (bool) (*IO_PIN_ARRAY[input_configs[i].port_index] & 1<<input_configs[i].pin_index);
    if (input && !pin_input_states[i])
      construct_publish_packet(input_configs[i].topic, zero_one + (input_configs[i].inverted ? 0 : 1), 1, true);
    else if (!input && pin_input_states[i])
      construct_publish_packet(input_configs[i].topic, zero_one + (input_configs[i].inverted ? 1 : 0), 1, true);
    pin_input_states[i] = input;
  }
}

static void autoc4_publish_callback(char const *topic,
    uint16_t topic_length, const void *payload, uint16_t payload_length)
{
  if (payload_length > 0)
  {
    // Set digital outputs
    for (int i=0; i<OUTPUT_COUNT; i++)
      if (strncmp(topic, output_configs[i].topic, topic_length) == 0)
      {
        if (((uint8_t*)payload)[0])
          *IO_PORT_ARRAY[output_configs[i].port_index] |= 1<<output_configs[i].pin_index;
        else
          *IO_PORT_ARRAY[output_configs[i].port_index] &= ~(1<<output_configs[i].pin_index);
        return;
      }

    // Set raw DMX channels
    if (strncmp(topic, CONF_AUTOC4_DMX_TOPIC, topic_length) == 0)
    {
      set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, payload_length);
      return;
    }

    // Set individual DMX devices
    for (int i=0; i<DMX_COUNT; i++)
      if (strncmp(topic, dmx_configs[i].topic, topic_length) == 0)
      {
        if (payload_length > dmx_configs[i].channel_count)
          payload_length = dmx_configs[i].channel_count;
        for (int j=0; j < payload_length; j++)
          set_dmx_channel(AUTOC4_DMX_UNIVERSE, dmx_configs[i].start_channel - 1 + j, ((uint8_t*)payload)[j]);
        return;
      }
  }
}

static void autoc4_ddr_init(void)
{
  // Enable outputs
  for (int i=0; i<OUTPUT_COUNT; i++)
    *IO_DDR_ARRAY[output_configs[i].port_index] |= 1<<input_configs[i].pin_index;

  // Enable pullups
  for (int i=0; i<INPUT_COUNT; i++)
    if (input_configs[i].pullup)
      *IO_PORT_ARRAY[input_configs[i].port_index] |= 1<<input_configs[i].pin_index;
}


void
autoc4_init(void)
{
  autoc4_ddr_init();
  mqtt_set_connection_config(&mqtt_con_config);
}

/*
  -- Ethersex META --
  header(services/autoc4/autoc4.h)
  init(autoc4_init)
*/
