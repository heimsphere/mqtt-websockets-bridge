default: all

all: bridge message-parser

bridge:
	$(MAKE) -C src

message-parser:
	$(MAKE) -C src/message-parser
	
format:
	uncrustify -c ../uncrustify.linux.cfg --replace *.c
	
memcheck:
	leaks `pidof bridge` 

.PHONY: clean
clean:
	$(MAKE) -C src cleanall

# static pattern rule (for tab completion sake)
# see http://www.gnu.org/software/make/manual/html_node/Static-Usage.html#Static-Usage 
