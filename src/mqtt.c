#include "bridge.h"

void
my_message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message)
{
  if (message->payloadlen)
  {
      llog(LOG_INFO, "Received message %s : %s\n", message->topic, message->payload);
      Subscription *subscription = subscription_get(&SUBSCRIPTIONS, message->topic);

      if (subscription)
      {
      	int i;
		int count =subscription->count_subscribed;
		llog(LOG_INFO, "Notify %d lws clients for topic %s\n", count, message->topic);
		for(i = 0; i < count; i++) {
			struct libwebsocket *wsi = subscription->subscribers[i];
			libwebsocket_callback_on_writable(WEBSOCKETS, wsi);
		}
      }
      else
      {
      	llog(LOG_ERR, "No lws clients are subscribed to topic %s\n", message->topic);
      }
  }
  else
  {
    printf("%s (null)\n", message->topic);
  }
}

void
my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
  if (!result)
    llog(LOG_INFO, "MQTT: connection established.\n");
  else
    llog(LOG_ERR, "MQTT: connection failed.\n");
}

void
my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
    int qos_count, const int *granted_qos)
{
  int i;

  printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
  for (i = 1; i < qos_count; i++)
    printf(", %d", granted_qos[i]);
  printf("\n");
}

void
my_log_callback(struct mosquitto *mosq, void *userdata, int level,
    const char *str)
{
  /* Pring all log messages regardless of level. */
  printf("%s\n", str);
}

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

  if (mosquitto_connect(mosq, host, port, keepalive))
    {
      llog(LOG_ERR, "MQTT: Unable to connect.\n");
      return NULL;
    }
  return mosq;
}
