#include <mosquitto.h>
#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h> /* required for log level definitions (required by llog) */
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h> /* gettimeofday */
#include <stdbool.h>
#include "subscriber.h"
#include "llog.h"
#include "message.h"

/* mqtt interface */
struct mosquitto *
initialize_mqtt_context(const char *id);

/* lws interface */
struct libwebsocket_context *
initialize_lws_context(struct lws_context_creation_info *info);

/* lws extpoll definitions */
#ifdef EXTERNAL_POLL
  extern int max_poll_elements;
  extern struct pollfd *pollfds;
  extern int *fd_lookup;
  extern int count_pollfds;
#endif

extern struct libwebsocket_protocols protocols[];

// TODO encapsulate in methods as static variables (replace initialize method?)
extern struct mosquitto *MOSQUITTO;
extern struct libwebsocket_context *WEBSOCKETS;
extern Subscriptions SUBSCRIPTIONS;

extern void
my_connect_callback(struct mosquitto *mosq, void *userdata, int result);

extern void
my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
    int qos_count, const int *granted_qos);

extern void
my_message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message);

extern void
my_log_callback(struct mosquitto *mosq, void *userdata, int level,
    const char *str);
