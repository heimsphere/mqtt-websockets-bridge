#include <mosquitto.h>
#include "mqtt.h"

#define RPC_REQUEST_PREFIX "_RPC/in"
#define RPC_REQUEST_PREFIX_LENGTH 7

#define RPC_RESPONSE_PREFIX "_RPC/out"
#define RPC_RESPONSE_PREFIX_LENGTH 8

#define RPC_SUBSCRIBE_PREFIX RPC_REQUEST_PREFIX "/+/+"

#define RPC_REQUEST_RESPONSE_OFFSET \
  RPC_RESPONSE_PREFIX_LENGTH - RPC_REQUEST_PREFIX_LENGTH

#define RPC_RESPONSE_TOPIC_LENGTH(message)\
    strlen(message->topic) + RPC_REQUEST_RESPONSE_OFFSET * sizeof(char)

#define RPC_RESPONSE_TOPIC(buf, message)\
  sprintf(buf, RPC_RESPONSE_PREFIX "%s", &(message->topic)[RPC_REQUEST_PREFIX_LENGTH])


/* TODO register service */

typedef struct service_method_t
{
  char *name;
  void(*cb)(MessageService *service, Message *message);
  ServiceMethod *next;
} ServiceMethod;

typedef struct message_service_t
{
  char *id;
  MessageQueue queue;
  ServiceMethod *first;
} MessageService;

/* linked list of parsed query parameters */

typedef struct param_t {
  char *key;
  char *value;
  QueryParam *next;
} QueryParam;

typedef struct query_parameter_t
{
  QueryParam *first;
  QueryParam *last;
  int count;
} QueryParams;

typedef struct service_request_t
{
  struct mosquitto *mosq;
  QueryParams *params;
} Request;


void
ServiceMethod_new(char *name, void
(*cb)(MessageService *service, Message *message));

void
ServiceMethod_free(ServiceMethod *method);

void
MessageService_new(MessageService *service, char *id);

void
MessageService_on(MessageService *service, const char *method, void
(*cb)(MessageService *service, Request *request));

void
MessageService_start(MessageService *service);

void
MessageService_free(MessageService *service);
