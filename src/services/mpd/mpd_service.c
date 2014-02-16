#include "service.h"
#include <time.h>

static void
echo(MessageService *service, Message *message)
{
  reply(message, message->data);
}

static void
clock(MessageService *service, Message *message)
{
  char date[100];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  strftime(date, sizeof(date)-1, "%d %m %Y %H:%M", t);

  reply(message, date);
}

int main(int argc, char **argv)
{
  char *name = argc == 2 ? argv[1] : "echo";
  MessageService service;
  MessageService_new(&service, name);
  /* do other settings ... */

  MessageService_on(&service, "echo", echo);
  MessageService_on(&service, "clock", clock);

  MessageService_timer(&service);

  MessageService_start(&service);
  MessageService_free(&service);
}
