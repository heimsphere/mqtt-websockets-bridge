
#line 1 "message_fsm.rl"
#include "message.h"


#line 54 "message_fsm.rl"



void
ragel_parse_message(MachineState *state, Message *message, bool eof)
{

#line 15 "message_fsm.c"
	static const char _message_fsm_actions[] = {
		0, 1, 1, 1, 4, 2, 0, 1,
		2, 0, 4, 2, 2, 1, 2, 3,
		1, 3, 0, 3, 1
	};

	static const char _message_fsm_key_offsets[] = {
		0,  0,	10,  20,  21,  34,  39, 43,
		45, 50, 51,  51
	};

	static const char _message_fsm_trans_keys[] = {
		10, 13,	 45,   95,    48,    57,    65,	  90,
		97, 122, 10,   13,    45,    95,    48,	  57,
		65, 90,	 97,   122,   10,    9,	    32,	  45,
		58, 95,	 11,   13,    48,    57,    65,	  90,
		97, 122, 9,    32,    58,    11,    13,	  10,
		32, 9,	 13,   10,    13,    10,    13,	  32,
		9,  12,	 10,   0
	};

	static const char _message_fsm_single_lengths[] = {
		0, 4, 4, 1, 5, 3, 2, 2,
		3, 1, 0, 0
	};

	static const char _message_fsm_range_lengths[] = {
		0, 3, 3, 0, 4, 1, 1, 0,
		1, 0, 0, 0
	};

	static const char _message_fsm_index_offsets[] = {
		0,  0,	8,  16,	 18,  28,  33,	37,
		40, 45, 47, 48
	};

	static const char _message_fsm_indicies[] = {
		0,  2,	3,   3,	  3,   3,   3,	 1,
		4,  5,	3,   3,	  3,   3,   3,	 1,
		4,  1,	6,   6,	  7,   8,   7,	 6,
		7,  7,	7,   1,	  9,   9,   10,	 9,
		1,  1,	12,  12,  11,  14,  15,	 13,
		14, 16, 12,  12,  11,  0,   1,	 17,
		18, 0
	};

	static const char _message_fsm_trans_targs[] = {
		2, 0,  9,  4,  10,  3,	 5,  4,
		6, 5,  6,  7,  8,   7,	 2,  7,
		8, 11, 11
	};

	static const char _message_fsm_trans_actions[] = {
		1,  0,	1,  5,	1,  1, 11, 1,
		11, 1,	1,  5,	5,  1, 14, 14,
		17, 5,	1
	};

	static const char _message_fsm_eof_actions[] = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 8, 3
	};

	static const int message_fsm_start = 1;
	static const int message_fsm_first_final = 10;
	static const int message_fsm_error = 0;

	static const int message_fsm_en_main = 1;


#line 61 "message_fsm.rl"

#line 88 "message_fsm.c"
	{
		state->cs = message_fsm_start;
	}

#line 62 "message_fsm.rl"
	state->p = &(message->data)[0];
	state->pe = &(message->data)[message->size - 1];
	if (eof)
		state->eof = state->pe;


#line 99 "message_fsm.c"
	{
		int _klen;
		unsigned int _trans;
		const char *_acts;
		unsigned int _nacts;
		const char *_keys;

		if ( ( state->p) == ( state->pe) )
			goto _test_eof;
		if (  state->cs == 0 )
			goto _out;
_resume:
		_keys = _message_fsm_trans_keys + _message_fsm_key_offsets[ state->cs];
		_trans = _message_fsm_index_offsets[ state->cs];

		_klen = _message_fsm_single_lengths[ state->cs];
		if ( _klen > 0 )
		{
			const char *_lower = _keys;
			const char *_mid;
			const char *_upper = _keys + _klen - 1;
			while (1)
			{
				if ( _upper < _lower )
					break;

				_mid = _lower + ((_upper - _lower) >> 1);
				if ( (*( state->p)) < *_mid )
					_upper = _mid - 1;
				else if ( (*( state->p)) > *_mid )
					_lower = _mid + 1;
				else
				{
					_trans += (unsigned int)(_mid - _keys);
					goto _match;
				}
			}
			_keys += _klen;
			_trans += _klen;
		}

		_klen = _message_fsm_range_lengths[ state->cs];
		if ( _klen > 0 )
		{
			const char *_lower = _keys;
			const char *_mid;
			const char *_upper = _keys + (_klen << 1) - 2;
			while (1)
			{
				if ( _upper < _lower )
					break;

				_mid = _lower + (((_upper - _lower) >> 1) & ~1);
				if ( (*( state->p)) < _mid[0] )
					_upper = _mid - 2;
				else if ( (*( state->p)) > _mid[1] )
					_lower = _mid + 2;
				else
				{
					_trans += (unsigned int)((_mid - _keys) >> 1);
					goto _match;
				}
			}
			_trans += _klen;
		}

_match:
		_trans = _message_fsm_indicies[_trans];
		state->cs = _message_fsm_trans_targs[_trans];

		if ( _message_fsm_trans_actions[_trans] == 0 )
			goto _again;

		_acts = _message_fsm_actions + _message_fsm_trans_actions[_trans];
		_nacts = (unsigned int)*_acts++;
		while ( _nacts-- > 0 )
		{
			switch ( *_acts++ )
			{
			case 0:
#line 12 "message_fsm.rl"
				{
					if (state->debug)
						printf("Mark[%ld] %c -> %p\n",
						       state->token_count, *(state->p), state->p);
					state->mark = state->p;
					state->mark_offset = 0;
				}
				break;
			case 1:
#line 20 "message_fsm.rl"
				{
					state->token_count++;
					state->mark_offset++;
				}
				break;
			case 2:
#line 25 "message_fsm.rl"
				{
					if (state->cb)
						state->cb(state, message, HEADER_NAME);
				}
				break;
			case 3:
#line 29 "message_fsm.rl"
				{
					if (state->cb)
						state->cb(state, message, HEADER_VALUE);
				}
				break;
#line 202 "message_fsm.c"
			}
		}

_again:
		if (  state->cs == 0 )
			goto _out;
		if ( ++( state->p) != ( state->pe) )
			goto _resume;
_test_eof: {}
		if ( ( state->p) == ( state->eof) )
		{
			const char *__acts = _message_fsm_actions + _message_fsm_eof_actions[ state->cs];
			unsigned int __nacts = (unsigned int)*__acts++;
			while ( __nacts-- > 0 )
			{
				switch ( *__acts++ )
				{
				case 0:
#line 12 "message_fsm.rl"
					{
						if (state->debug)
							printf("Mark[%ld] %c -> %p\n",
							       state->token_count, *(state->p), state->p);
						state->mark = state->p;
						state->mark_offset = 0;
					}
					break;
				case 4:
#line 33 "message_fsm.rl"
					{
						if (state->cb)
							state->cb(state, message, BODY);
					}
					break;
#line 234 "message_fsm.c"
				}
			}
		}

_out: {}
	}

#line 67 "message_fsm.rl"

	if (state->p != state->pe)
		printf("Only consumed %lu of %lu tokens: %p -> [%c]\n",
		       state->token_count, message->size - 1, state->p, *(state->p));

}
