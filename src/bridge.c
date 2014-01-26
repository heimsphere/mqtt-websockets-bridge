#include "bridge.h"

#define EVENT_LOOP_TIMEOUT 10

struct mosquitto *MOSQUITTO;
struct libwebsocket_context *WEBSOCKETS;
Subscriptions SUBSCRIPTIONS;

int
main(const int argc, const char *argv[])
{
  assert(argc == 2);
  const char *id = argv[1];

  llog_init();

  subscriptions_new(&SUBSCRIPTIONS);

#ifdef EXTERNAL_POLL
  max_poll_elements = getdtablesize();
  pollfds = malloc(max_poll_elements * sizeof(struct pollfd));
  fd_lookup = malloc(max_poll_elements * sizeof(int));
  if (pollfds == NULL || fd_lookup == NULL)
    {
      lwsl_err("Out of memory pollfds=%d\n", max_poll_elements);
      return -1;
    }
#endif

  struct lws_context_creation_info info;
  WEBSOCKETS = initialize_lws_context(&info);
  if (!WEBSOCKETS)
    {
      llog(LOG_ERR, "Server Starting\n");
      return -1;
    }

  MOSQUITTO = initialize_mqtt_context(id);
  if (!MOSQUITTO)
    {
      llog(LOG_ERR, "Failed to initialize mosquitto context\n");
      return -1;
      /* TODO if this is a connection error try to reconnect */
    }

  llog(LOG_INFO, "Starting server\n");

  for (;;)
    {
#ifdef EXTERNAL_POLL
      int n = 0;
      /*
       * this represents an existing server's single poll action
       * which also includes libwebsocket sockets
       */

      n = poll(pollfds, count_pollfds, 50);
      if (n < 0)
      continue;

      if (n)
      for (n = 0; n < count_pollfds; n++)
      if (pollfds[n].revents)
      /*
       * returns immediately if the fd does not
       * match anything under libwebsockets
       * control
       */
      if (libwebsocket_service_fd(WEBSOCKETS, &pollfds[n]) < 0)
      lwsl_notice("No socket needs LWS service");
#else
      libwebsocket_service(WEBSOCKETS, EVENT_LOOP_TIMEOUT);
#endif
      int rc = mosquitto_loop(MOSQUITTO, EVENT_LOOP_TIMEOUT, 1);
      if (rc != MOSQ_ERR_SUCCESS)
        llog(LOG_ERR, "Error in mosquitto event loop: %s\n",
            mosquitto_strerror(rc));
    }

  libwebsocket_context_destroy(WEBSOCKETS);
  lwsl_notice("Server Shutdown down\n");
  mosquitto_destroy(MOSQUITTO);
  mosquitto_lib_cleanup();
  subscriptions_destroy(&SUBSCRIPTIONS);
  return 0;
}
