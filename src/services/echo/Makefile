echo_service: clean
        $(CC) -g -pedantic -Wall -o echo_service mqtt.c llog.c echo_service.c \
                -I$(MOSQUITTO_PATH)/include/ -L$(MOSQUITTO_PATH)/lib -lmosquitto
                