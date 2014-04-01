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


static void
autoc4_connack_callback(void)
{
  autoc4_autoconf_connack_callback();
}

static void
autoc4_poll(void)
{
  autoc4_autoconf_poll();
}

static void
autoc4_publish_callback(char const *topic, uint16_t topic_length,
    const void *payload, uint16_t payload_length)
{
  autoc4_autoconf_publish_callback(topic, topic_length,
      payload, payload_length);
}


void
autoc4_init(void)
{
  autoc4_autoconf_ddr_init();
  mqtt_set_connection_config(&mqtt_con_config);
}

/*
  -- Ethersex META --
  header(services/autoc4/autoc4.h)
  init(autoc4_init)
*/
