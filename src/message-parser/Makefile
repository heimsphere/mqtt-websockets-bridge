NUM ?= 1000000
OBJ=message.o message_fsm.o message-parser.o
CFLAGS= -Os -g -DNUM=$(NUM) -DTRACE $(SRC)

default: message-parser

.PHONY: clean
clean:
	rm -f *.o message-parser
	
message-parser: $(OBJ)

# pattern rules
# see https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html#Pattern-Rules

%.dot : %.rl
	ragel -V -p -o $@ $<

%.png : %.dot
	dot -Tpng $< -o $@

%.c : %.rl
	ragel -o $@ $<