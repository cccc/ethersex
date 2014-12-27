#include "protocols/uip/uip.h"

static autoc4_output_config outputs[] = {
  { PORT_INDEX_A, 0, "licht/fnord/links",  false },
  { PORT_INDEX_A, 1, "licht/fnord/rechts", false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_D, 2, "schalter/fnord/links", true, false },
};
static autoc4_dmx_config dmxs[] = {
  { "dmx/fnord/scummfenster",   1, 4 },
  { "dmx/fnord/schranklinks",   5, 4 },
  { "dmx/fnord/schrankrechts",  9, 4 },
  { "dmx/fnord/fairyfenster",  13, 4 },
};
static char const* const auto_subscribe_topics[] = { "licht/fnord/+","dmx/fnord/+","dmx/fnord", NULL };
static mqtt_connection_config_t mqtt_config = {
  .client_id = "fnordcenter",
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
  .output_count = 2,
  .input_count = 1,
  .dmx_count = 4,
  .output_configs = outputs,
  .input_configs = inputs,
  .dmx_configs = dmxs,
  .dmx_topic = "dmx/fnord",
  .mqtt_con_config = &mqtt_config,
};
