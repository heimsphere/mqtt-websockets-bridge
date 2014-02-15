#include "message.h"

%%{
	machine message_fsm;
	
	action Print {
		if (state->debug)
			printf("Token[%ld] %c -> %p\n", 
				state->token_count, *(state->p), state->p);
	}
	
	action Mark {
		if (state->debug)
			printf("Mark[%ld] %c -> %p\n", 
				state->token_count, *(state->p), state->p);
		state->mark = state->p;
		state->mark_offset = 0;
	}
	
	action Count {
		state->token_count++;
		state->mark_offset++;
	}
	
	action HeaderName {
		if (state->cb) state->cb(state, message, HEADER_NAME);
	}
	
	action HeaderValue {
		if (state->cb) state->cb(state, message, HEADER_VALUE);
	}
	
	action Body {
		if (state->cb) state->cb(state, message, BODY);
	}
	
	# keep machine state in a struct
	access state->;
	variable p state->p;
	variable pe state->pe;
	variable eof state->eof;
	
	SEP = '\r'? '\n';
	inline = (any -- SEP);
	header_name = ( (alnum | '-' | '_') -- ':')+;
	# strips leading and trailing whitespace
	header_value = inline+; 
	header = (( header_name  >Mark %HeaderName ) space* ':' space* 
			   ( header_value >Mark %HeaderValue ))  -- SEP;
	envelope = header? ( SEP header )*; 
	body = any*;
	
 	main :=  ( envelope SEP{2} (body >Mark %Body)) $Count; 
}%%


void 
ragel_parse_message(MachineState *state, Message *message, bool eof)
{
	%% write data;
	%% write init;
	state->p = &(message->data)[0];
	state->pe = &(message->data)[message->size-1];
	if (eof) state->eof = state->pe;
	
	%% write exec;
	
	if (state->p != state->pe) {
		printf("Only consumed %lu of %lu tokens: %p -> [%c]\n", 
			state->token_count, message->size-1, state->p, *(state->p));
	}
		
}
