#include "protocols/uip/uip.h"

static autoc4_output_config outputs[] = {
  { PORT_INDEX_C, 3, "licht/wohnzimmer/kueche",       false },
  { PORT_INDEX_C, 4, "licht/wohnzimmer/mitte",        false },
  { PORT_INDEX_C, 5, "licht/wohnzimmer/tuer",         false },
  { PORT_INDEX_D, 2, "licht/wohnzimmer/gang",         false },
  { PORT_INDEX_D, 6, "power/wohnzimmer/kitchenlight", false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_C, 6, "schalter/wohnzimmer/links",  false, false },
  { PORT_INDEX_C, 7, "schalter/wohnzimmer/rechts", false, false },
  { PORT_INDEX_D, 4, "schalter/wohnzimmer/gang",   true,  false },
};
static autoc4_dmx_config dmxs[] = {
  { "dmx/wohnzimmer/mitte1",   1, 8 },
  { "dmx/wohnzimmer/mitte2",   9, 8 },
  { "dmx/wohnzimmer/mitte3",  17, 8 },
};
static char const* const auto_subscribe_topics[] = { "licht/wohnzimmer/+","dmx/wohnzimmer/+","dmx/wohnzimmer","power/wohnzimmer/+", NULL };
static mqtt_connection_config_t mqtt_config = {
  .client_id = "wohnzimmer",
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
  .dmx_topic = "dmx/wohnzimmer",
  .mqtt_con_config = &mqtt_config,
};
