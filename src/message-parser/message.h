#include <string.h> /* memcpy, strerror, */
#include <stdio.h> /* fprintf, feof, fopen, FILE */
#include <stdlib.h> /* free, exit, malloc */
#include <stdbool.h> /* true, false */

typedef enum parse_event_t
{
  HEADER_NAME, HEADER_VALUE, BODY
} ParseEvent;

static const char * const Parse_Event_Str[] =
  { "HEADER_NAME", "HEADER_VALUE", "BODY" };

typedef struct header_t
{
  char *name;
  char *value;
  struct header_t *next;
} Header;

typedef struct header_list_t
{
  Header *first;
  Header *last;
  int count;
} Envelope;

typedef struct parsed_message_t
{
  char *data;
  char *body;
  size_t size;
  Envelope envelope;
} Message;

typedef struct state_machine_t
{
  char *p;
  char *pe;
  char *eof;
  int res;
  int cs;
  char *mark;
  char *mark_offset;
  long token_count;
  bool debug;
  void
  (*cb)(struct state_machine_t *state, Message *message, ParseEvent event);
} MachineState;

void
Message_new(Message *message);

void
Header_new(Header *header, char *name, char*value);

void
Envelope_add_header(Envelope *envelope, char *name, char *value);

void
Envelope_fwrite(Envelope *envelope, FILE *stream);

void
Message_free(Message *message);

void
Message_parse(Message *message, char *buf, size_t size);

void
ragel_parse_message(MachineState *state, Message *message, bool eof);
