#include "mqtt.h"

int
MessageQueue_new(MessageQueue *queue, const char *id)
{
  queue->host = "127.0.0.1";
  queue->port = 1883;
  queue->keepalive = 60;
  queue->clean_session = true;
  queue->queue = NULL;
  queue->reconnect_attempts = 0;

  queue->on_connect = NULL;
  queue->on_message = NULL;
  queue->on_log = NULL;
  queue->on_subscribe = NULL;

  mosquitto_lib_init();
  queue->queue = mosquitto_new(id, queue->clean_session, NULL);

  if (queue->queue == NULL)
    {
      llog(LOG_ERR, "MQTT: %s.\n", strerror(errno));
      return -1;
    }
  return 1;
}

int
MessageQueue_connect(MessageQueue *queue)
{
  mosquitto_log_callback_set(queue->queue, queue->on_log);
  mosquitto_connect_callback_set(queue->queue, queue->on_connect);
  mosquitto_message_callback_set(queue->queue, queue->on_message);
  mosquitto_subscribe_callback_set(queue->queue, queue->on_subscribe);

  int rc = mosquitto_connect(queue->queue, queue->host, queue->port,
      queue->keepalive);
  if (rc != MOSQ_ERR_SUCCESS)
    {
      if (rc == MOSQ_ERR_ERRNO)
        llog(LOG_ERR, "MQTT: %s.\n", strerror(rc));
      else
        llog(LOG_ERR, "MQTT: %s.\n", mosquitto_strerror(rc));
    }
  return rc;
}

void
MessageQueue_free(MessageQueue *queue)
{
  mosquitto_destroy(queue->queue);
  mosquitto_lib_cleanup();
}

void
MessageQueue_run(MessageQueue *queue, int timeout)
{
  int rc = mosquitto_loop(queue->queue, timeout, 1);
  if (rc == MOSQ_ERR_SUCCESS)
    queue->reconnect_attempts = 0;
  else
    {
      if (rc == MOSQ_ERR_CONN_LOST || rc == MOSQ_ERR_CONN_REFUSED
          || rc == MOSQ_ERR_NO_CONN)
        {
          if (queue->reconnect_attempts == 0)
            llog(LOG_WARNING, "Mosquitto trying reconnect: %s\n",
                mosquitto_strerror(rc));
          mosquitto_reconnect(queue->queue);
          queue->reconnect_attempts++;
        }
      else
        llog(LOG_ERR, "Error in mosquitto event loop: %s\n",
            mosquitto_strerror(rc));
    }
}
