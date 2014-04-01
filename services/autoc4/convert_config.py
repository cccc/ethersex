#!/bin/python

# Copyright (c) 2014 by Philip Matura <ike@tura-home.de>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# For more information on the GPL, please go to:
# http://www.gnu.org/copyleft/gpl.html

"""
Usage:

    python convert_config.py config.json output.c pinning.m4
"""

import re
import sys
import json
from django.template import Template, Context
from django.conf import settings as d_settings


TARGETFILE_TEMPLATE = """
#ifndef AUTOC4_AUTO_CONF_H
#define AUTOC4_AUTO_CONF_H

#include "protocols/uip/uip.h"
#include "services/dmx-storage/dmx_storage.h"

#define True 1
#define False 0

#define CONF_AUTOC4_MQTT_IP HTONS((({{mqtt_ip.0}}) << 8) | ({{mqtt_ip.1}})), HTONS((({{mqtt_ip.2}}) << 8) | ({{mqtt_ip.3}}))
#define CONF_AUTOC4_MQTT_CLIENTID {{mqtt_client_id}}
#define CONF_AUTOC4_MQTT_USER {{mqtt_user}}
#define CONF_AUTOC4_MQTT_PASS {{mqtt_pass}}
#define CONF_AUTOC4_MQTT_AUTOSUBS {{mqtt_autosubs}}

static const uint8_t zero_one[2] = { 0, 1 };

{% for pin, topic, pupd, invert in inputs %}
  static uint8_t pin_{{pin}}_state;
{% endfor %}

static inline void autoc4_autoconf_connack_callback(void)
{
  {% for pin, topic, pull_up, invert in inputs %}
    if (PIN_HIGH({{pin}}))
    {
      construct_publish_packet({{topic}}, zero_one + ({{invert}} ? 0 : 1), 1, true);
      pin_{{pin}}_state = 1;
    }
    else
    {
      construct_publish_packet({{topic}}, zero_one + ({{invert}} ? 1 : 0), 1, true);
      pin_{{pin}}_state = 0;
    }
  {% endfor %}
}

static void autoc4_autoconf_poll(void)
{
  {% for pin, topic, pull_up, invert in inputs %}
    if (PIN_HIGH({{pin}}) && !pin_{{pin}}_state)
    {
      construct_publish_packet({{topic}}, zero_one + ({{invert}} ? 0 : 1), 1, true);
      pin_{{pin}}_state = 1;
    }
    else if (!PIN_HIGH({{pin}}) && pin_{{pin}}_state)
    {
      construct_publish_packet({{topic}}, zero_one + ({{invert}} ? 1 : 0), 1, true);
      pin_{{pin}}_state = 0;
    }
  {% endfor %}
}

static void autoc4_autoconf_publish_callback(char const *topic,
    uint16_t topic_length, const void *payload, uint16_t payload_length)
{
  if (payload_length > 0)
  {
    if (0) { }

    {% for pin, topic in outputs %}
      else if (strncmp(topic, {{topic}}, topic_length) == 0)
      {
        if (((uint8_t*)payload)[0])
        {
          PIN_SET({{pin}});
        }
        else
        {
          PIN_CLEAR({{pin}});
        }
      }
    {% endfor %}

    {% if dmx_topic %}
      else if (strncmp(topic, {{dmx_topic}}, topic_length) == 0)
      {
        set_dmx_channels(payload, AUTOC4_DMX_UNIVERSE, payload_length);
      }
    {% endif %}

    {% for topic, start_channel, channel_count in dmx %}
      else if (strncmp(topic, {{topic}}, topic_length) == 0)
      {
        if (payload_length > {{channel_count}})
          payload_length = {{channel_count}};
        for (int i=0; i < payload_length; i++)
          set_dmx_channel(AUTOC4_DMX_UNIVERSE, {{start_channel}} - 1 + i, ((uint8_t*)payload)[i]);
      }
    {% endfor %}
  }
}

static void autoc4_autoconf_ddr_init(void)
{
  // It seems the following can be done (prettier?) and automatically by the
  // pinning subsystem. If anyone knows how to use it, please tell me.

  {% for port_char, pin_nr in outputs_parsed %}
    DDR{{port_char}} |= 1<<{{pin_nr}};
  {% endfor %}

  {% for port_char, pin_nr, pupd in inputs_parsed %}
    {% if pupd %}
      PORT{{port_char}} |= 1<<{{pin_nr}};
    {% endif %}
  {% endfor %}
}

#endif
"""

PINNING_TEMPLATE = """
{% for pin, topic in outputs %}
pin({{pin}}, {{pin}}, OUTPUT)
{% endfor %}
{% for pin, topic, pupd, invert in inputs %}
pin({{pin}}, {{pin}}, INPUT)
{% endfor %}
"""


def main():
    if len(sys.argv) < 4:
        print(__doc__)
        exit(1)

    with open(sys.argv[1], 'rb') as f:
        settings = json.load(f)

    d_settings.configure()


    # convert settings

    mqtt_ip = [int(s) for s in settings['mqtt_ip'].split('.')]

    mqtt_clientid = '"{}"'.format(settings['mqtt_client_id'])

    if settings['mqtt_user'] is None:
        mqtt_user = 'NULL'
    else:
        mqtt_user = '"{}"'.format(settings['mqtt_user'])

    if settings['mqtt_pass'] is None:
        mqtt_pass = 'NULL'
    else:
        mqtt_pass = '"{}"'.format(settings['mqtt_pass'])

    mqtt_autosubs = ','.join(['"{}"'.format(s) for s in settings['mqtt_auto_subscribe']])

    parse_pin_name = lambda m: (m.group(1), m.group(2))
    inputs_parsed = [parse_pin_name(re.match(r'P([A-F])([0-9])', pin)) + (pupd,) for pin, topic, pupd, invert in settings['inputs']]
    outputs_parsed = [parse_pin_name(re.match(r'P([A-F])([0-9])', pin)) for pin, topic in settings['outputs']]
    inputs = [(pin, '"{}"'.format(topic), pupd, invert) for pin, topic, pupd, invert in settings['inputs']]
    outputs = [(pin, '"{}"'.format(topic)) for pin, topic in settings['outputs']]
    dmx = [('"{}"'.format(topic), start_channel, channel_count) for topic, start_channel, channel_count in settings['dmx']]
    dmx_topic = '"{}"'.format(settings['dmx_topic'])

    with open(sys.argv[2], 'wb') as targetfile:
        targetfile.write(Template(TARGETFILE_TEMPLATE).render(Context({
            'mqtt_ip': mqtt_ip,
            'mqtt_client_id': mqtt_clientid,
            'mqtt_user': mqtt_user,
            'mqtt_pass': mqtt_pass,
            'mqtt_autosubs': mqtt_autosubs,
            'outputs': outputs,
            'inputs': inputs,
            'outputs_parsed': outputs_parsed,
            'inputs_parsed': inputs_parsed,
            'dmx_topic': dmx_topic,
            'dmx': dmx,
        }, autoescape=False)))

    with open(sys.argv[3], 'wb') as pinningfile:
        pinningfile.write(Template(PINNING_TEMPLATE).render(Context({
            'outputs': outputs,
            'inputs': inputs,
        }, autoescape=False)))


if __name__ == '__main__':
    main()
