#include <stdlib.h> /* NULL */
#include <stdio.h>

typedef enum
{
  SUBSCRIBE, UNSUBSCRIBE, PUBLISH
} Method;

typedef struct message_t
{
  Method method;
  const char *topic;
  const void *data;
  const void *serialized;
  int size;
} Message;

void
message_new(Message *msg, Method method, const char *topic, const void *data);

int
message_parse(Message *msg, void *data);

void
message_serialize(Message *msg);

void
message_free(Message *msg);


