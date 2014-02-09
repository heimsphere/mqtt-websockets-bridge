#include <stdlib.h> /* NULL */
#include <stdio.h>
#include <string.h>

typedef enum
{
  SUBSCRIBE, /*!< subscribe to a topic */
  UNSUBSCRIBE, /*!< unsubscribe from a topic */
  PUBLISH /*!< publish a message to a topic */
} Method;

/*!
 * The message
 */
typedef struct message_t
{
  Method method;
  const char *topic; /*!< the topic */
  const char *data;
  char *serialized;
  int size;
} Message;

/*!
 * Creates a new message.
 *
 * @param[out] msg The pointer to the message that is created.
 * @param[in] method The method of the message.
 * @param[in] topic The path of the queue to which the message is send (e.g /foo/bar).
 * @param[in] data The payload for the message.
 */
void
message_new(Message *msg, Method method, const char *topic, const char *data);

/*!
 * Parses an incomming message.
 *
 * @param[out] msg The pointer to the message that is created.
 * @param[in] data The serialized message.
 */
int
message_parse(Message *msg, const char *data);

/*!
 * Serializes the given message.
 * The serialized message is stored in the message itself.
 *
 * @param[in,out] msg The message which is serialized.
 */
void
message_serialize_request(Message *msg);

/*!
 * Serializes the given message.
 * The serialized message is stored in the message itself.
 *
 * @param[in,out] msg The message which is serialized.
 */
void
message_serialize_response(Message *msg);


/*!
 * Frees the memory allocated for the message, including the serialized form.
 *
 * @param[in] msg The message to free.
 */
void
message_free(Message *msg);
