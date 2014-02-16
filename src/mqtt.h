#include "llog.h"
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <mosquitto.h>
#include <string.h>

/*
 * From MQTT 3.1 spec:
 *
 * The topic name is a UTF-encoded string.
 * See the section on MQTT and UTF-8 for more information.
 * Topic name has an upper length limit of 32,767 characters.
 *
 * ~> 32k
 */
#define MAX_TOPIC_LENGTH 32767


typedef struct message_queue_t
{
  char *id;
  const char *host;
  int port;
  int keepalive;
  bool clean_session;
  struct mosquitto *queue;
  int reconnect_attempts;
  void (*on_log)(struct mosquitto *, void *, int, const char *);
  void (*on_connect)(struct mosquitto *, void *, int);
  void (*on_message)(struct mosquitto *, void *, const struct mosquitto_message *);
  void (*on_subscribe)(struct mosquitto *, void *, int, int, const int *);
} MessageQueue;


int
MessageQueue_new(MessageQueue *queue, const char *id);

int
MessageQueue_connect(MessageQueue *queue);

void
MessageQueue_free(MessageQueue *queue);

void
MessageQueue_run(MessageQueue* queue, int timeout);
