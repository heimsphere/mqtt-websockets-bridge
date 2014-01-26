LIBWS_PATH=`brew --prefix libwebsockets`
LIBEV_PATH=`brew --prefix libev`
MOSQUITTO_PATH=`brew --prefix mosquitto`
SRC=llog.c subscriber.c ws.c mqtt.c bridge.c

ALL=bridge

default: $(ALL)

clean:
	rm -f $(ALL)

format:
	uncrustify -c ../uncrustify.linux.cfg --replace *.c
	
memcheck:
	leaks `pidof bridge`


bridge: clean
	gcc -g -pedantic -Wall -o bridge $(SRC) \
		-DINSTALL_DATADIR='"."' -DEXTERNAL_POLL \
		-I$(LIBWS_PATH)/include/ -L$(LIBWS_PATH)/lib -lwebsockets \
		-I$(MOSQUITTO_PATH)/include/ -L$(MOSQUITTO_PATH)/lib -lmosquitto
