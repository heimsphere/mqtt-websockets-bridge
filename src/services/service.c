#include "service.h"

static void
connect_callback(struct mosquitto *mosq, void *userdata, int result)
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
message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message)
{
    llog(LOG_INFO, "received message from : %s\n", message->topic);
    char response_topic[RPC_RESPONSE_TOPIC_LENGTH(message)];
    RPC_RESPONSE_TOPIC(response_topic, message);
    llog(LOG_INFO, "sending response to: %s\n", response_topic);
    mosquitto_publish(mosq, 0, response_topic, message->payloadlen, message->payload, 0, false);
}

void
MessageService_new(MessageService *service, char *id)
{
  service->id = id;
  MessageQueue_new(&(service->queue));
  service->queue.on_connect = connect_callback;
  service->queue.on_message = message_callback;
}
