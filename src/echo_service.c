#include <mosquitto.h>
#include "bridge.h"

void
my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
  mosquitto_subscribe(mosq, 0, "_RPC/in/+/+/echo/echo", 0);
}

void
my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
    int qos_count, const int *granted_qos)
{

}

#define OUT_PREFIX "_RPC/out"
#define OUT_PREFIX_LEN 7

extern void
my_message_callback(struct mosquitto *mosq, void *userdata,
    const struct mosquitto_message *message)
{
    char buf[512];
    char *in_topic = strdup(&(message->topic)[OUT_PREFIX_LEN]);
    sprintf(buf, OUT_PREFIX "%s", in_topic);
    printf("%s\n", buf);
    mosquitto_publish(mosq, 0, buf, message->payloadlen, message->payload, 0, false);
    free(in_topic);
}

extern void
my_log_callback(struct mosquitto *mosq, void *userdata, int level,
    const char *str) {

}

int main(int argc, char **argv)
{
  struct mosquitto *mosq = NULL;
  mosq = initialize_mqtt_context("echo");
  for(;;)
    mosquitto_loop_forever(mosq, -1, 1);
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
}
