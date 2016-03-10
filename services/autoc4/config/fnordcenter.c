#include "protocols/uip/uip.h"

static const char string_output_1[]  PROGMEM = "licht/fnord/links";
static const char string_output_2[]  PROGMEM = "licht/fnord/rechts";

static const char string_input_1[]   PROGMEM = "schalter/fnord/links";

static const char string_dmx_1[]     PROGMEM = "dmx/fnord/scummfenster";
static const char string_dmx_2[]     PROGMEM = "dmx/fnord/schranklinks";
static const char string_dmx_3[]     PROGMEM = "dmx/fnord/schrankrechts";
static const char string_dmx_4[]     PROGMEM = "dmx/fnord/fairyfenster";

static const char string_autosub_1[] PROGMEM = "licht/fnord/+";
static const char string_autosub_2[] PROGMEM = "dmx/fnord/+";
static const char string_autosub_3[] PROGMEM = "dmx/fnord";

static const char string_client_id[] PROGMEM = "fnordcenter";

static autoc4_output_config outputs[] = {
  { PORT_INDEX_A, 0, string_output_1, false, false },
  { PORT_INDEX_A, 1, string_output_2, false, false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_D, 2, string_input_1, true, false },
};
static autoc4_dmx_config dmxs[] = {
  { string_dmx_1,  1, 4 },
  { string_dmx_2,  5, 4 },
  { string_dmx_3,  9, 4 },
  { string_dmx_4, 13, 4 },
};
static PGM_P const auto_subscribe_topics[] = {
  string_autosub_1,
  string_autosub_2,
  string_autosub_3,
  NULL
};
static mqtt_connection_config_t mqtt_config = {
  .client_id = string_client_id,
  .user = NULL,
  .pass = NULL,
  .will_topic = NULL,
  .will_qos = 0,
  .will_retain = 0,
  .will_message = NULL,
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
  .dmx_topic = "dmx/fnord",
  .mqtt_con_config = &mqtt_config,
};
