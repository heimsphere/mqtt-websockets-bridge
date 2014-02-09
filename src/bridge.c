#include "bridge.h"

#define EVENT_LOOP_TIMEOUT 10

struct mosquitto *MOSQUITTO;
struct libwebsocket_context *WEBSOCKETS;
Subscriptions SUBSCRIPTIONS;

void
client_unsubscribed(Subscriptions *subscriptions, Subscription *sub,
    void *subscriber)
{
  llog(LOG_INFO, "Unsubscribe client %p from topic %s\n", subscriber,
      sub->topic);

  if (sub->count_subscribed == 0)
    {
      llog(LOG_INFO, "No more subscribers left for topic %s. "
          "Unsubscribe from server.\n", sub->topic);
      mosquitto_unsubscribe(MOSQUITTO, NULL, sub->topic);
    }
}

static void
my_message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message)
{
  if (message->payloadlen)
    {
      llog(LOG_INFO, "Received message %s : %s\n", message->topic,
          message->payload);
      Subscription *subscription = subscription_get(&SUBSCRIPTIONS,
          message->topic);
      if (subscription)
        {
          llog(LOG_INFO, "Notify %d lws clients for topic %s\n",
              subscription->count_subscribed, message->topic);

          Message msg;
          message_new(&msg, PUBLISH, message->topic, message->payload);
          message_serialize(&msg);

          // create libwebsockets message from MQTT payload
          unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + msg.size
              + LWS_SEND_BUFFER_POST_PADDING];
          unsigned char *lws_message = &buf[LWS_SEND_BUFFER_PRE_PADDING];
          int lws_message_length = sprintf((char * )lws_message, "%s",
              msg.serialized);

          // dispatch message to all subscribers
          for (int i = 0; i < subscription->count_subscribed; i++)
            {
              struct libwebsocket *wsi = subscription->subscribers[i];
              int bytes_written = libwebsocket_write(wsi, lws_message,
                  lws_message_length, LWS_WRITE_TEXT);
              if (bytes_written < lws_message_length)
                {
                  llog(LOG_ERR,
                      "ERROR only %d bytes written (message length is %d)\n",
                      bytes_written, lws_message_length);
                }
            }
          message_free(&msg);
        }
      else
        {
          llog(LOG_ERR, "No lws clients are subscribed to topic %s\n",
              message->topic);
        }
    }
  else
    {
      printf("%s (null)\n", message->topic);
    }
}

static void
my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
  if (!result)
    llog(LOG_INFO, "MQTT: connection established.\n");
  else
    llog(LOG_ERR, "MQTT: connection failed.\n");
}

static void
my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
    int qos_count, const int *granted_qos)
{
  int i;

  printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
  for (i = 1; i < qos_count; i++)
    printf(", %d", granted_qos[i]);
  printf("\n");
}

static void
my_log_callback(struct mosquitto *mosq, void *userdata, int level,
    const char *str)
{
  /* Pring all log messages regardless of level. */
  printf("%s\n", str);
}

int
main(const int argc, const char *argv[])
{
  assert(argc == 2);
  const char *id = argv[1];

  llog_init();

  subscriptions_new(&SUBSCRIPTIONS);
  SUBSCRIPTIONS.unsubscribe_cb = client_unsubscribed;

#ifdef EXTERNAL_POLL
  max_poll_elements = getdtablesize();
  pollfds = malloc(max_poll_elements * sizeof(struct pollfd));
  fd_lookup = malloc(max_poll_elements * sizeof(int));
  if (pollfds == NULL || fd_lookup == NULL)
    {
      lwsl_err("Out of memory pollfds=%d\n", max_poll_elements);
      return -1;
    }
#endif

  struct lws_context_creation_info info;
  WEBSOCKETS = initialize_lws_context(&info);
  if (!WEBSOCKETS)
    {
      llog(LOG_ERR, "Server Starting\n");
      return -1;
    }

  MessageQueue queue;
  if (!MessageQueue_new(&queue, id))
      llog(LOG_ERR, "Failed to create the MessageQueue.\n");

  queue.on_message = my_message_callback;
  queue.on_connect = my_connect_callback;
  queue.on_log = my_log_callback;
  queue.on_subscribe = my_subscribe_callback;

  MessageQueue_connect(&queue);
  MOSQUITTO = queue.queue;

  llog(LOG_INFO, "Starting server\n");

  for (;;)
    {
#ifdef EXTERNAL_POLL
      int n = 0;
      /*
       * this represents an existing server's single poll action
       * which also includes libwebsocket sockets
       */

      n = poll(pollfds, count_pollfds, 50);
      if (n < 0)
      continue;

      if (n)
      for (n = 0; n < count_pollfds; n++)
      if (pollfds[n].revents)
      /*
       * returns immediately if the fd does not
       * match anything under libwebsockets
       * control
       */
      if (libwebsocket_service_fd(WEBSOCKETS, &pollfds[n]) < 0)
      lwsl_notice("No socket needs LWS service");
#else
      libwebsocket_service(WEBSOCKETS, EVENT_LOOP_TIMEOUT);
#endif
      MessageQueue_run(&queue, EVENT_LOOP_TIMEOUT);
    }

  libwebsocket_context_destroy(WEBSOCKETS);
  lwsl_notice("Server Shutdown down\n");
  MessageQueue_free(&queue);
  subscriptions_destroy(&SUBSCRIPTIONS);
  return 0;
}
