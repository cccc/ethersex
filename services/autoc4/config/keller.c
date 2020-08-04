#include "protocols/uip/uip.h"

static const char string_output_1[]  PROGMEM = "licht/keller/mitte";
static const char string_output_2[]  PROGMEM = "licht/keller/loet";
static const char string_output_3[]  PROGMEM = "licht/keller/vorne";
static const char string_output_4[]  PROGMEM = "led/keller/werkbankkalt";
static const char string_output_5[]  PROGMEM = "led/keller/werkbankwarm";
static const char string_output_6[]  PROGMEM = "led/keller/hintenkalt";
static const char string_output_7[]  PROGMEM = "led/keller/hintenwarm";

static const char string_input_1[]   PROGMEM = "schalter/keller/1";
static const char string_input_2[]   PROGMEM = "schalter/keller/2";
static const char string_input_3[]   PROGMEM = "schalter/keller/3";
static const char string_input_4[]   PROGMEM = "schalter/keller/4";
static const char string_input_5[]   PROGMEM = "schalter/keller/hinten1";
static const char string_input_6[]   PROGMEM = "schalter/keller/hinten2";

static const char string_autosub_1[] PROGMEM = "licht/keller/+";
static const char string_autosub_2[] PROGMEM = "led/keller/+";
static const char string_autosub_3[] PROGMEM = "etherrape/keller/+";

static const char string_client_id[] PROGMEM = "keller";
static const char string_heartbeat_topic[] PROGMEM = "heartbeat/keller";
static const char string_logicer_heartbeat_topic[] PROGMEM = "heartbeat/logicer";
static const char string_subscribe_sudo[] PROGMEM = "sudo/#";
static const uint8_t will_message[]  = { 0x00 };

static const char string_fan_topic[] PROGMEM = "etherrape/keller/fan";
static const char string_status_led_topic[] PROGMEM = "etherrape/keller/statusled";
static const char string_locked_mode_topic[] PROGMEM = "etherrape/keller/lock";


static autoc4_output_config outputs[] = {
  { PORT_INDEX_D, 0, string_output_1, false, false, true,  false },
  { PORT_INDEX_D, 4, string_output_2, false, false, true,  false },
  { PORT_INDEX_D, 5, string_output_3, false, false, true,  false },
  { PORT_INDEX_D, 2, string_output_4, false, true,  false, true  },
  { PORT_INDEX_D, 6, string_output_5, false, true,  false, true  },
  { PORT_INDEX_C, 4, string_output_6, false, true,  false, true  },
  { PORT_INDEX_C, 0, string_output_7, false, true,  false, true  },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_A, 4, string_input_1, true, false, true  },
  { PORT_INDEX_A, 0, string_input_2, true, false, true  },
  { PORT_INDEX_A, 5, string_input_3, true, false, false },
  { PORT_INDEX_A, 1, string_input_4, true, false, false },
  { PORT_INDEX_A, 7, string_input_5, true, false, false },
  { PORT_INDEX_A, 3, string_input_6, true, false, false },
};
static autoc4_dmx_config dmxs[] = {
};
static PGM_P const auto_subscribe_topics[] = {
  string_autosub_1,
  string_autosub_2,
  string_autosub_3,
  string_logicer_heartbeat_topic,
  string_subscribe_sudo,
  NULL
};
static mqtt_connection_config_t mqtt_config = {
  .client_id = string_client_id,
  .user = NULL,
  .pass = NULL,
  .will_topic = string_heartbeat_topic,
  .will_qos = 0,
  .will_retain = 1,
  .will_message = will_message,
  .will_message_length = sizeof(will_message),
  .target_ip = { HTONS(((172) << 8) | (23)), HTONS(((23) << 8) | (110)) },

  .auto_subscribe_topics = auto_subscribe_topics,
};
static autoc4_config_t config = {
  .output_count = sizeof(outputs) / sizeof(autoc4_output_config),
  .input_count = sizeof(inputs) / sizeof(autoc4_input_config),
  .dmx_count = sizeof(dmxs) / sizeof(autoc4_dmx_config),
  .output_configs = outputs,
  .input_configs = inputs,
  .dmx_configs = dmxs,
  .dmx_topic = NULL,
  .mqtt_con_config = &mqtt_config,
};
