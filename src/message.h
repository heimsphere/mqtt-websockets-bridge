#include <stdlib.h> /* NULL */
#include <stdio.h>
#include <string.h>

typedef enum
{
  SUBSCRIBE, UNSUBSCRIBE, PUBLISH
} Method;

typedef struct message_t
{
  Method method;
  const char *topic;
  const char *data;
  char *serialized;
  int size;
} Message;

void
message_new(Message *msg, Method method, const char *topic, const char *data);

int
message_parse(Message *msg, const char *data);

void
message_serialize(Message *msg);

void
message_free(Message *msg);
