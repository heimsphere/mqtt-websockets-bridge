#include <mosquitto.h>
#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h> /* for log level definition (until syslog is used) */
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h> /* gettimeofday */
#include <stdbool.h>
#include "subscriber.h"

/* mqtt interface */
struct mosquitto *
initialize_mqtt_context(const char *id);

/* lws interface */
struct libwebsocket_context *
initialize_lws_context(struct lws_context_creation_info *info);

/* lws extpoll definitions */
extern int max_poll_elements;

extern struct pollfd *pollfds;
extern int *fd_lookup;
extern int count_pollfds;

/* logger interface */
void
llog_init();

void
llog(int level, const char *fmt, ...);

// TODO store topic tokenized
// check whether a subscribed topic matches a published topic
// mosquitto_sub_topic_tokenise("$SYS/broker/uptime", &topics, &topic_count);
/* mosquitto_sub_topic_tokens_free */
/* mosquitto_topic_matches_sub */

extern struct libwebsocket_protocols protocols[];

enum demo_protocols
{
  /* always first */
  PROTOCOL_HTTP = 0,

  PROTOCOL_DUMB_INCREMENT, PROTOCOL_LWS_MIRROR, PROTOCOL_MQTT_BRIDGE,

  /* always last */
  DEMO_PROTOCOL_COUNT
};

// FIXME do not declare it globally
extern struct mosquitto *MOSQUITTO;
extern Subscriptions SUBSCRIPTIONS;
extern struct libwebsocket_context *WEBSOCKETS;
extern char *PAYLOAD;


