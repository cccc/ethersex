#include "protocols/uip/uip.h"

static autoc4_output_config outputs[] = {
  { PORT_INDEX_D, 5, "licht/keller/aussen", false, false },
  { PORT_INDEX_D, 7, "licht/keller/innen",  false, false },
  { PORT_INDEX_C, 0, "licht/keller/vorne",  false, false },
};
static autoc4_input_config inputs[] = {
  { PORT_INDEX_C, 1, "schalter/keller/1", true, false },
  { PORT_INDEX_C, 2, "schalter/keller/2", true, false },
};
static autoc4_dmx_config dmxs[] = {
};
static char const* const auto_subscribe_topics[] = { "licht/keller/+", NULL };
static mqtt_connection_config_t mqtt_config = {
  .client_id = "keller",
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
  .dmx_topic = NULL,
  .mqtt_con_config = &mqtt_config,
};
