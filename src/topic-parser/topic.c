#include "topic.h"

void
MachineState_new(MachineState *state, char *buf)
{
	state->res = 0;
	state->cs = 0;
	// state->p = argv[1];
	// state->pe = state->p + strlen(state->p) + 1;
	state->pe = NULL;
	state->start = NULL;
	state->end = NULL;
	state->eof = NULL;
	state->buf = buf;
	state->tcount = 0;
}

void
ParsedTopic_new(ParsedTopic *topic, char *data)
{
	topic->_data = NULL;
	topic->scheme = NULL;
	topic->path = NULL;
	topic->fragment = NULL;
	topic->param = NULL;
	topic->last_param = NULL;
	topic->_data = data;
}

void
Param_new(Param *param)
{
	param->key = NULL;
	param->value = NULL;
	param->next = NULL;
}

void
ParsedTopic_new_param(ParsedTopic *topic, char *key)
{
	Param *param = calloc(sizeof(Param), 1);

	Param_new(param);
	param->key = key;

	if (topic->param == NULL)
	{
		topic->param = param;
		topic->last_param = param;
	}
	else
	{
		topic->last_param->next = param;
		topic->last_param = param;
	}
}

void
ParsedTopic_free(ParsedTopic *topic)
{
	Param *p = topic->param;

	while (p != NULL)
	{
		Param *to_free = p;
		p = to_free->next;
		free(to_free);
	}
	free(topic->_data);
}

void
ParsedTopic_pretty_print(ParsedTopic *topic)
{
	printf("scheme: [%s]\n", topic->scheme);
	printf("path: [%s]\n", topic->path);
	printf("fragment: [%s]\n", topic->fragment);

	Param *param = topic->param;

	int i = 1;
	while (param)
	{
		printf("param[%d]: %s -> %s\n", i, param->key, param->value);
		param = param->next;
		i++;
	}
}

void
ParsedTopic_parse(ParsedTopic *topic, char *buf, size_t len)
{
	MachineState state;

	// using malloc is faster than strdup
	state.buf = malloc(len + 1);
	memcpy(state.buf, buf, len + 1);
	MachineState_new(&state, state.buf);
//      MachineState_new(&state, strdup(buf));
	ParsedTopic_new(topic, state.buf);
	tokenize_topic(&state, topic, len, true);
}
