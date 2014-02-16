#include "message.h"

void
MachineState_new(MachineState *state)
{
	state->res = 0;
	state->cs = 0;
	state->pe = NULL;
	state->mark = NULL;
	state->eof = NULL;
	state->cb = NULL;
	state->token_count = 0;
	state->mark_offset = 0;
	state->debug = false;
}

void
Message_new(Message *message)
{
	message->data = NULL;
	message->body = NULL;
	message->size = 0;
	message->envelope.first = NULL;
	message->envelope.last = NULL;
	message->envelope.count = 0;
}

void
Header_new(Header *header, char *name, char*value)
{
	header->name = name;
	header->value = value;
	header->next = NULL;
}

void
Envelope_add_header(Envelope *envelope, char *name, char *value)
{
	Header *header = malloc(sizeof(Header));

	if (envelope->last)
	{
		envelope->last->next = header;
		envelope->last = header;
	}
	else
	{
		envelope->first = header;
		envelope->last = header;
	}
	Header_new(header, name, value);
}

void
Message_free(Message *message)
{
	Header *header = message->envelope.first;

	while (header)
	{
		free(header);
		header = header->next;
	}
	free(message->data);
}

void
Envelope_fwrite(Envelope *envelope, FILE *stream)
{
	Header *header = envelope->first;

	while (header)
	{
		fprintf(stream, "%s: %s\n", header->name, header->value);
		header = header->next;
	}
}

void
cb(MachineState *state, Message *message, ParseEvent event)
{
	if (state->debug)
		printf("Event %s, current token %ld \n",
		       Parse_Event_Str[(int)event], state->token_count);

	switch (event)
	{
	case HEADER_NAME:
		Envelope_add_header(&(message->envelope), state->mark, NULL);
		*(state->p) = '\0';
		break;
	case HEADER_VALUE:
		message->envelope.last->value = state->mark;
		*(state->p) = '\0';
		break;
	case BODY:
		message->body = state->mark;
		break;
	}
}

void
Message_parse(Message *message, char *buf, size_t size)
{
	MachineState state;

	MachineState_new(&state);
	state.cb = cb;
	Message_new(message);
	// using malloc is faster than strdup
	//      MachineState_new(&state, strdup(buf));
//  state.debug = true;
	message->data = malloc(size);
	memcpy(message->data, buf, size);
	message->size = size;
	ragel_parse_message(&state, message, true);
}
