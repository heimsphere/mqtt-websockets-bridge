#include "mqtt.h"
// TODO register all resources (called by client.c)

int main(int argc, char **argv)
{
  MessageQueue queue;
  MessageQueue_new(&queue, "echo");
//  queue.on_message = my_message_callback;
//  queue.on_connect = my_connect_callback;
  MessageQueue_connect(&queue);
  for(;;)
    MessageQueue_run(&queue, 1000);
  MessageQueue_free(&queue);
}
