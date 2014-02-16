MOSQUITTO_PATH=`brew --prefix mosquitto`
MPDCLIENT_PATH=`brew --prefix libmpdclient`
SRC=mpdc.c mpd-mqtt-client.c

ALL=mpd-mqtt-client

default: $(ALL)

clean:
	rm -f $(ALL)

format:
	uncrustify -c ../uncrustify.linux.cfg --replace *.c
	
memcheck:
	leaks `pidof mpd-mqtt-client`

mpd-mqtt-client: clean
	$(CC) -g -pedantic -Wall -o mpd-mqtt-client $(SRC) \
		-I$(MOSQUITTO_PATH)/include -L$(MOSQUITTO_PATH)/lib -lmosquitto \
		-I$(MPDCLIENT_PATH)/include -L$(MPDCLIENT_PATH)/lib -lmpdclient
