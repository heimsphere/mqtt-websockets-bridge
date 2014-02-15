NUM ?= 1000000
OBJ=topic.o topic_fsm.o topic-parser.o
CFLAGS= -Os -g -DTRACE $(SRC)

default: topic-parser

.PHONY: clean
clean:
	rm -f *.o topic-parser
	
topic-parser: $(OBJ)

# pattern rules
# see https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html#Pattern-Rules

%.dot : %.rl
	ragel -V -p -o $@ $<

%.png : %.dot
	dot -Tpng $< -o $@

%.c : %.rl
	ragel -o $@ $<