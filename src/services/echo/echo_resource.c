#include "service.h"

static void
my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
  if (!result)
    {
     llog(LOG_INFO, "MQTT: connection established.\n");
     mosquitto_subscribe(mosq, 0, RPC_SUBSCRIBE_PREFIX "/echo/echo", 0);
    }
   else
     llog(LOG_ERR, "MQTT: connection failed.\n");
}

static void
my_message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message)
{
    llog(LOG_INFO, "received message from : %s\n", message->topic);
    char response_topic[RPC_RESPONSE_TOPIC_LENGTH(message)];
    RPC_RESPONSE_TOPIC(response_topic, message);
    llog(LOG_INFO, "sending response to: %s\n", response_topic);
    mosquitto_publish(mosq, 0, response_topic, message->payloadlen, message->payload, 0, false);
}
