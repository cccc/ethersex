#include "protocols/uip/uip.h"

static autoc4_output_config outputs[] = {
  { PORT_INDEX_A, 4, "licht/plenar/vornefenster",  false, false },
  { PORT_INDEX_A, 5, "licht/plenar/vornewand",     false, false },
  { PORT_INDEX_A, 6, "licht/plenar/hintenfenster", false, false },
  { PORT_INDEX_A, 7, "licht/plenar/hintenwand",    false, false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_D, 2, "schalter/plenar/vorne",       false, false },
  { PORT_INDEX_D, 3, "fenster/plenar/vornerechts",  true,  false },
  { PORT_INDEX_D, 4, "fenster/plenar/vornelinks",   true,  false },
  { PORT_INDEX_D, 5, "fenster/plenar/hintenrechts", true,  false },
  { PORT_INDEX_D, 6, "fenster/plenar/hintenlinks",  true,  false },
};
static autoc4_dmx_config dmxs[] = {
  { "dmx/plenar/vorne1",   1, 8 },
  { "dmx/plenar/vorne2",   9, 8 },
  { "dmx/plenar/vorne3",  17, 8 },
  { "dmx/plenar/hinten1", 25, 8 },
  { "dmx/plenar/hinten2", 33, 8 },
  { "dmx/plenar/hinten3", 41, 8 },
  { "dmx/plenar/hinten4", 49, 8 },
};
static char const* const auto_subscribe_topics[] = { "licht/plenar/+","dmx/plenar/+","dmx/plenar", NULL };
static mqtt_connection_config_t mqtt_config = {
  .client_id = "plenarsaal",
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
  .dmx_topic = "dmx/plenar",
  .mqtt_con_config = &mqtt_config,
};
