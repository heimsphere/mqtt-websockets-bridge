#include "topic.h"

/* machine definition */

/* callback(MachineState *s) */

%%{
	machine topic_fsm;
	
	action Mem {
		printf("Mem %c -> %p\n", *(state->p), state->p);
	}
	
	action Tok {
		*(state->p) = '\0';
	}
	
	action ParamAdd {
		ParsedTopic_new_param(topic, state->p);
	}
	
	action ValueAdd {
		topic->last_param->value = state->p;
	}
	
	action SchemeSet {
		topic->scheme = state->p;
	}
	
	action PathSet {
		topic->path = state->p;
	}
	
	action FragmentSet {
		topic->fragment = state->p;
	}
	
	# action Error {
	# 	printf("parsing error at token %lu : %p\n", state->tcount, state->p);
	# }
	
	# action CheckConsumedAll {
	# 	if (state->p != state->pe)
	# 		printf("Only consumed %lu tokens\n", state->tcount);
	# }
	
	action Count {
		state->tcount++;
	}
	
	# keep machine state in a struct
	access state->;
	variable p state->p;
	variable pe state->pe;
	variable eof state->eof;
	
	idf = alnum;
	
	# TODO we can collect cut positions and cut later if no error occured ;)
	
	scheme = (idf -- ':' )+ >SchemeSet %Tok @^{printf("invalid scheme\n");};
	path = (('/' idf+)+ '/'?)+ >PathSet %Tok @^{printf("invalid path\n");} ;	# does not allow empty path levels
	fragment = (idf -- '?')+ >FragmentSet %Tok @^{printf("invalid fragment\n");};
	param_name = idf+ >ParamAdd %Tok;
	param_value = (any -- space -- '&')+ >ValueAdd %Tok;
	param = param_name '=' param_value;	
	params = ( '?' param ) ( '&' param )*;
 	
 	main :=  (scheme ':' path ('#' fragment)?  params?)  $Count;
}%%


void tokenize_topic(MachineState *state, ParsedTopic *topic, size_t len, bool eof)
{
	%% write data;
	%% write init;
	state->p = &(state->buf)[0];
	state->pe = &(state->buf)[len-1];
	if (eof) state->eof = state->pe;
	
	%% write exec;
	
	if (state->p != state->pe) {
		printf("Only consumed %lu of %lu tokens: %p -> [%c]\n", 
			state->tcount, len, state->p, *(state->p));
	}
		
}
