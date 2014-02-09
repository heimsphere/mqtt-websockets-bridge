#include "message.h"

#define REQUEST_FORMAT "%s %s\n%s"
#define RESPONSE_FORMAT "%s\n%s"

static const char *const method_str[] =
  { "SUBSCRIBE", "UNSUBSCRIBE", "PUBLISH" };

static const int method_str_size = 3;

static int
message_request_size(Message *msg);

static int
message_response_size(Message *msg);

void
message_new(Message *msg, Method method, const char *topic, const char *data)
{
  msg->method = method;
  msg->topic = strdup(topic);
  if (data) msg->data = strdup((char *) data);
  msg->size = 0;
  msg->serialized = NULL;
}

static int
message_parse_method(Message *message, const char *method)
{
  int i;
  for(i = 0; i < method_str_size; i++)
    {
      if (strcmp(method_str[i], method) == 0)
        {
          message->method = (Method) i;
          return 1;
        }
    }
  return -1;
}

/*
 * message must be freed (on success and on error)
 */
int
message_parse(Message *msg, const char *data)
{
  char *data_copy;

  /* copy data, since strtok replaces the tokens */
  data_copy = strdup(data);

  msg->topic = NULL;
  msg->data = NULL;
  msg->serialized = NULL;
  msg->size = 0;

  char *token = data_copy;
  char *rest = data_copy;
  strsep(&rest, " ");

 /* first token is the method */
 int res = message_parse_method(msg, token);

 if (res < 0)
   {
     printf("Unknown method in message : %s\n", token);
     free(data_copy);
     return -1;
   }

  token = rest;
  strsep(&rest, "\n");

  msg->topic = strdup(token);
  if (rest && strlen(rest) > 0) msg->data = strdup(rest);
  free(data_copy);

  return 1;
}

void
message_serialize_request(Message *msg)
{
  free(msg->serialized);
  msg->size = message_request_size(msg);
  msg->serialized = malloc(msg->size);

  snprintf(msg->serialized, msg->size,
      REQUEST_FORMAT,
      method_str[msg->method],
      msg->topic,
      msg->data);
}

void
message_serialize_response(Message *msg)
{
  free(msg->serialized);
  msg->size = message_response_size(msg);
  msg->serialized = malloc(msg->size);

  snprintf(msg->serialized, msg->size,
      RESPONSE_FORMAT,
      msg->topic,
      msg->data);
}

static int
message_request_size(Message *msg)
{
  return
      (strlen(method_str[msg->method])
      + strlen(msg->topic)
      + ((msg->data) ? strlen(msg->data) : 0)
      + 3) /* separators + \0 terminator */
      * sizeof(char);
}

static int
message_response_size(Message *msg)
{
  return
      (strlen(msg->topic)
      + ((msg->data) ? strlen(msg->data) : 0)
      + 2) /* 1 separator + \0 terminator */
      * sizeof(char);
}

void
message_free(Message *msg)
{
  free((void *) msg->topic);
  free((void *) msg->data);
  free(msg->serialized);

  msg->topic = NULL;
  msg->data = NULL;
  msg->serialized = NULL;
  msg->size = 0;
}
