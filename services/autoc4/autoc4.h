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

#ifndef AUTOC4_H
#define AUTOC4_H


#include <stdbool.h>


#include "config.h"

#ifndef AUTOC4_MQTT_SUPPLY_USER
  #undef CONF_AUTOC4_MQTT_USER
  #define CONF_AUTOC4_MQTT_USER NULL
#endif

#ifndef AUTOC4_MQTT_SUPPLY_PASS
  #undef CONF_AUTOC4_MQTT_PASS
  #define CONF_AUTOC4_MQTT_PASS NULL
#endif


#if IO_HARD_PORTS == 3
  #error Not implemented for this uC type
#endif

#define PORT_INDEX_A 0
#define PORT_INDEX_B 1
#define PORT_INDEX_C 2
#define PORT_INDEX_D 3
#define PORT_INDEX_E 4
#define PORT_INDEX_F 5
#define PORT_INDEX_G 6


typedef struct {
  uint8_t port_index, pin_index;
  const char* topic;
} autoc4_output_config;

typedef struct {
  uint8_t port_index, pin_index;
  const char* topic;
  bool pullup, inverted;
} autoc4_input_config;

typedef struct {
  const char* topic;
  uint16_t start_channel, channel_count;
} autoc4_dmx_config;


typedef struct {
  uint8_t output_count;
  uint8_t input_count;
  uint8_t dmx_count;
  autoc4_output_config *output_configs;
  autoc4_input_config *input_configs;
  autoc4_dmx_config *dmx_configs;
  const char *dmx_topic;
  mqtt_connection_config_t *mqtt_con_config;
} autoc4_config_t;


void autoc4_init(void);


#endif /* MOTD_H */
