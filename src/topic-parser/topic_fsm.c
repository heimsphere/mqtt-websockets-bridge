
#line 1 "topic_fsm.rl"
#include "topic.h"

/* machine definition */

/* callback(MachineState *s) */


#line 70 "topic_fsm.rl"



void tokenize_topic(MachineState *state, ParsedTopic *topic, size_t len, bool eof)
{

#line 18 "topic_fsm.c"
	static const char _topic_fsm_actions[] = {
		0, 1, 0, 1, 6, 1, 7, 1,
		8, 1, 9, 2, 0, 6, 2, 1,
		6, 2, 2, 6, 2, 3, 6, 2,
		4, 6, 2, 5, 6
	};

	static const char _topic_fsm_key_offsets[] = {
		0,  0,	6,  13,	 14,  20,  26,	32,
		39, 43, 52, 59,	 63
	};

	static const char _topic_fsm_trans_keys[] = {
		48, 57,	 65,   90,    97,     122,     58,	48,
		57, 65,	 90,   97,    122,    47,      48,	57,
		65, 90,	 97,   122,   48,     57,      65,	90,
		97, 122, 48,   57,    65,     90,      97,	122,
		61, 48,	 57,   65,    90,     97,      122,	32,
		38, 9,	 13,   35,    47,     63,      48,	57,
		65, 90,	 97,   122,   63,     48,      57,	65,
		90, 97,	 122,  32,    38,     9,       13,	35,
		47, 63,	 48,   57,    65,     90,      97,	122,
		0
	};

	static const char _topic_fsm_single_lengths[] = {
		0, 0, 1, 1, 0, 0, 0, 1,
		2, 3, 1, 2, 3
	};

	static const char _topic_fsm_range_lengths[] = {
		0, 3, 3, 0, 3, 3, 3, 3,
		1, 3, 3, 1, 3
	};

	static const char _topic_fsm_index_offsets[] = {
		0,  0,	4,  9,	11,  15,  19,  23,
		28, 32, 39, 44, 48
	};

	static const char _topic_fsm_indicies[] = {
		1,  1,	1,  0,	 4,   2,    2,	  2,
		3,  6,	5,  7,	 7,   7,    5,	  9,
		9,  9,	8,  10,	 10,  10,   3,	  12,
		11, 11, 11, 3,	 3,   3,    3,	  13,
		14, 15, 16, 7,	 7,   7,    3,	  16,
		17, 17, 17, 3,	 3,   16,   3,	  18,
		14, 19, 16, 7,	 7,   7,    3,	  0
	};

	static const char _topic_fsm_trans_targs[] = {
		0, 2,  2,  0,  3,  0,	4,   9,
		0, 10, 7,  7,  8,  11,	5,   12,
		6, 10, 11, 4
	};

	static const char _topic_fsm_trans_actions[] = {
		5,  20,	 3,   0,   11,	 7,   23,   3,
		9,  26,	 14,  3,   11,	 17,  11,   3,
		11, 3,	 3,   3
	};

	static const char _topic_fsm_eof_actions[] = {
		0, 5, 0, 7, 7, 9, 0, 0,
		0, 1, 1, 1, 1
	};

	static const int topic_fsm_start = 1;
	static const int topic_fsm_first_final = 9;
	static const int topic_fsm_error = 0;

	static const int topic_fsm_en_main = 1;


#line 76 "topic_fsm.rl"

#line 95 "topic_fsm.c"
	{
		state->cs = topic_fsm_start;
	}

#line 77 "topic_fsm.rl"
	state->p = &(state->buf)[0];
	state->pe = &(state->buf)[len - 1];
	if (eof)
		state->eof = state->pe;


#line 106 "topic_fsm.c"
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
		_keys = _topic_fsm_trans_keys + _topic_fsm_key_offsets[ state->cs];
		_trans = _topic_fsm_index_offsets[ state->cs];

		_klen = _topic_fsm_single_lengths[ state->cs];
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

		_klen = _topic_fsm_range_lengths[ state->cs];
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
		_trans = _topic_fsm_indicies[_trans];
		state->cs = _topic_fsm_trans_targs[_trans];

		if ( _topic_fsm_trans_actions[_trans] == 0 )
			goto _again;

		_acts = _topic_fsm_actions + _topic_fsm_trans_actions[_trans];
		_nacts = (unsigned int)*_acts++;
		while ( _nacts-- > 0 )
		{
			switch ( *_acts++ )
			{
			case 0:
#line 14 "topic_fsm.rl"
				{
					*(state->p) = '\0';
				}
				break;
			case 1:
#line 18 "topic_fsm.rl"
				{
					ParsedTopic_new_param(topic, state->p);
				}
				break;
			case 2:
#line 22 "topic_fsm.rl"
				{
					topic->last_param->value = state->p;
				}
				break;
			case 3:
#line 26 "topic_fsm.rl"
				{
					topic->scheme = state->p;
				}
				break;
			case 4:
#line 30 "topic_fsm.rl"
				{
					topic->path = state->p;
				}
				break;
			case 5:
#line 34 "topic_fsm.rl"
				{
					topic->fragment = state->p;
				}
				break;
			case 6:
#line 47 "topic_fsm.rl"
				{
					state->tcount++;
				}
				break;
			case 7:
#line 61 "topic_fsm.rl"
				{ printf("invalid scheme\n"); }
				break;
			case 8:
#line 62 "topic_fsm.rl"
				{ printf("invalid path\n"); }
				break;
			case 9:
#line 63 "topic_fsm.rl"
				{ printf("invalid fragment\n"); }
				break;
#line 234 "topic_fsm.c"
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
			const char *__acts = _topic_fsm_actions + _topic_fsm_eof_actions[ state->cs];
			unsigned int __nacts = (unsigned int)*__acts++;
			while ( __nacts-- > 0 )
			{
				switch ( *__acts++ )
				{
				case 0:
#line 14 "topic_fsm.rl"
					{
						*(state->p) = '\0';
					}
					break;
				case 7:
#line 61 "topic_fsm.rl"
					{ printf("invalid scheme\n"); }
					break;
				case 8:
#line 62 "topic_fsm.rl"
					{ printf("invalid path\n"); }
					break;
				case 9:
#line 63 "topic_fsm.rl"
					{ printf("invalid fragment\n"); }
					break;
#line 268 "topic_fsm.c"
				}
			}
		}

_out: {}
	}

#line 82 "topic_fsm.rl"

	if (state->p != state->pe)
		printf("Only consumed %lu of %lu tokens: %p -> [%c]\n",
		       state->tcount, len, state->p, *(state->p));

}
