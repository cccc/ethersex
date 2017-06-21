#include "protocols/uip/uip.h"

static const char string_output_1[]  PROGMEM = "licht/plenar/vornefenster";
static const char string_output_2[]  PROGMEM = "licht/plenar/vornewand";
static const char string_output_3[]  PROGMEM = "licht/plenar/hintenfenster";
static const char string_output_4[]  PROGMEM = "licht/plenar/hintenwand";

static const char string_input_1[]   PROGMEM = "schalter/plenar/vorne";
static const char string_input_2[]   PROGMEM = "fenster/plenar/vornerechts";
static const char string_input_3[]   PROGMEM = "fenster/plenar/vornelinks";
static const char string_input_4[]   PROGMEM = "fenster/plenar/hintenrechts";
static const char string_input_5[]   PROGMEM = "fenster/plenar/hintenlinks";

static const char string_dmx_1[]     PROGMEM = "dmx/plenar/vorne1";
static const char string_dmx_2[]     PROGMEM = "dmx/plenar/vorne2";
static const char string_dmx_3[]     PROGMEM = "dmx/plenar/vorne3";
static const char string_dmx_4[]     PROGMEM = "dmx/plenar/hinten1";
static const char string_dmx_5[]     PROGMEM = "dmx/plenar/hinten2";
static const char string_dmx_6[]     PROGMEM = "dmx/plenar/hinten3";
static const char string_dmx_7[]     PROGMEM = "dmx/plenar/hinten4";

static const char string_autosub_1[] PROGMEM = "licht/plenar/+";
static const char string_autosub_2[] PROGMEM = "dmx/plenar/+";
static const char string_autosub_3[] PROGMEM = "dmx/plenar";

static const char string_client_id[] PROGMEM = "plenarsaal";
static const char string_heartbeat_topic[] PROGMEM = "heartbeat/plenarsaal";
static const char string_locked_mode_topic[] PROGMEM = "lock/plenar";
static const char string_logicer_heartbeat_topic[] PROGMEM = "heartbeat/logicer";
static const char string_subscribe_sudo[] PROGMEM = "sudo/#";
static const uint8_t will_message[]  = { 0x00 };


static autoc4_output_config outputs[] = {
  { PORT_INDEX_A, 4, string_output_1, false, false, true, false },
  { PORT_INDEX_A, 5, string_output_2, false, false, true, false },
  { PORT_INDEX_A, 6, string_output_3, false, false, true, false },
  { PORT_INDEX_A, 7, string_output_4, false, false, true, false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_D, 2, string_input_1, false, false, true  },
  { PORT_INDEX_D, 3, string_input_2, true,  false, false },
  { PORT_INDEX_D, 4, string_input_3, true,  false, false },
  { PORT_INDEX_D, 5, string_input_4, true,  false, false },
  { PORT_INDEX_D, 6, string_input_5, true,  false, false },
};
static autoc4_dmx_config dmxs[] = {
  { string_dmx_1,  1, 8 },
  { string_dmx_2,  9, 8 },
  { string_dmx_3, 17, 8 },
  { string_dmx_4, 25, 8 },
  { string_dmx_5, 33, 8 },
  { string_dmx_6, 41, 8 },
  { string_dmx_7, 49, 8 },
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
  .dmx_topic = "dmx/plenar",
  .mqtt_con_config = &mqtt_config,
};
