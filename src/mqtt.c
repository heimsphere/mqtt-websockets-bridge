#include "bridge.h"


struct mosquitto *
initialize_mqtt_context(const char *id)
{
  const char *host = "127.0.0.1";
  int port = 1883;
  int keepalive = 60;
  bool clean_session = true;

  mosquitto_lib_init();
  struct mosquitto *mosq = mosquitto_new(id, clean_session, NULL);
  if (!mosq)
    {
      llog(LOG_ERR, "MQTT: Out of memory.\n");
      return NULL;
    }
  mosquitto_log_callback_set(mosq, my_log_callback);
  mosquitto_connect_callback_set(mosq, my_connect_callback);
  mosquitto_message_callback_set(mosq, my_message_callback);
  mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

  int rc = mosquitto_connect(mosq, host, port, keepalive);
  if (rc != MOSQ_ERR_SUCCESS)
    {
      if (rc == MOSQ_ERR_ERRNO)
        llog(LOG_ERR, "MQTT: %s.\n", strerror(rc));
      else
        llog(LOG_ERR, "MQTT: %s.\n", mosquitto_strerror(rc));
      return NULL;
    }
  return mosq;
}
