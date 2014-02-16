SUBDIRS=message-parser topic-parser services
SUBDIRS_CLEAN = $(patsubst %,%.clean,$(SUBDIRS))
OBJ=llog.o subscriber.o ws.o mqtt.o message.o bridge.o
CFLAGS += -g -pedantic -Wall -DEXTERNAL_POLL
LDFLAGS = -lwebsockets -lmosquitto

default: all
	
bridge: $(OBJ)

.PHONY: clean
clean:
	rm -f *.o bridge

.PHONY: all
all: bridge $(SUBDIRS)

.PHONY: cleanall    
cleanall: clean $(SUBDIRS_CLEAN)
 
 .PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

%.clean:
	$(MAKE) -C $(@:.clean=) clean
