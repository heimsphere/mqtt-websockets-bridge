#include <mosquitto.h>
#include "mqtt.h"
#include "message.h"

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

typedef struct message_service_t MessageService;
typedef struct param_t QueryParam;
/*
 * Service Descriptor

{"jsonrpc": "2.0", "method": "register",  "id": 1, "params":
        {
                path : '/{service_path}/{uuid}'                    --> Path is the key
                api: '{api_url}'
                name: 'Rubens Player'
        }
}

 */
typedef struct service_descriptor_t
{
  char *path;
  char *uuid;
  char *api_url;
  char *name;
} ServiceDescriptor;

typedef struct service_resource_t
{
  char *name;
  void(*cb)(MessageService *service, Message *message);
  struct service_resource_t *next;
} Resource;

struct message_service_t
{
  char *id;
  MessageQueue queue;
  Resource *first;
};

/* linked list of parsed query parameters */

struct param_t {
  char *key;
  char *value;
  QueryParam *next;
};

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
ServiceMethod_free(Resource *method);

void
MessageService_new(MessageService *service, char *id);

void
MessageService_on(MessageService *service, const char *method, void
(*cb)(MessageService *service, Request *request));

void
MessageService_start(MessageService *service);

void
MessageService_free(MessageService *service);

void
MessageService_register(MessageService *service);
