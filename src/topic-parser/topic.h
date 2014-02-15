#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct state_machine_t
{
  char *p;
  char *pe;
  char *eof;
  char *buf;
  int res;
  int cs;
  char *start;
  char *end;
  long tcount;
} MachineState;

typedef struct param_t
{
  char *key;
  char *value;
  struct param_t *next;
} Param;

typedef struct parsed_topic_t
{
  char *_data;
  char *scheme;
  char *path;
  char*fragment;
  Param *param;
  Param *last_param;
} ParsedTopic;

void
MachineState_new(MachineState *state, char *buf);

void
ParsedTopic_new(ParsedTopic *topic, char *buf);

void
ParsedTopic_free(ParsedTopic *topic);

void
ParsedTopic_pretty_print(ParsedTopic *topic);

void
Param_new(Param *param);

void
ParsedTopic_new_param(ParsedTopic *topic, char *key);

void
ParsedTopic_parse(ParsedTopic *topic, char *buf, size_t len);

void
tokenize_topic(MachineState *state, ParsedTopic *topic, size_t len, bool eof);
