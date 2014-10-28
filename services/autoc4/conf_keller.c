#include "protocols/uip/uip.h"

static autoc4_output_config outputs[] = {
  { PORT_INDEX_D, 5, "licht/keller/aussen" },
  { PORT_INDEX_D, 7, "licht/keller/innen" },
};
static autoc4_input_config inputs[] = {
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
  .output_count = 2,
  .input_count = 0,
  .dmx_count = 0,
  .output_configs = outputs,
  .input_configs = inputs,
  .dmx_configs = dmxs,
  .dmx_topic = NULL,
  .mqtt_con_config = &mqtt_config,
};
