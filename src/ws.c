#include "bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <syslog.h>
#include <signal.h>

#include <libwebsockets.h>

int max_poll_elements;

struct pollfd *pollfds;
int *fd_lookup;
int count_pollfds;
int force_exit = 0;

#define LOCAL_RESOURCE_PATH "./"

/*
 * We take a strict whitelist approach to stop ../ attacks
 */

struct serveable
{
  const char *urlpath;
  const char *mimetype;
};

static const struct serveable whitelist[] =
  {
    { "/favicon.ico", "image/x-icon" },
    { "/libwebsockets.org-logo.png", "image/png" },

  /* last one is the default served if no match */
    { "/test.html", "text/html" }, };

struct per_session_data__http
{
  int fd;
};

/* this protocol server (always the first one) just knows how to do HTTP */

static int
callback_http(struct libwebsocket_context *context, struct libwebsocket *wsi,
    enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
#if 0
  char client_name[128];
  char client_ip[128];
#endif
  char buf[256];
  char leaf_path[1024];
  int n, m;
  unsigned char *p;
  static unsigned char buffer[4096];
  struct stat stat_buf;
  struct per_session_data__http *pss = (struct per_session_data__http *) user;
#ifdef EXTERNAL_POLL
  int fd = (int)(long)in;
#endif

  switch (reason)
    {
  case LWS_CALLBACK_HTTP:

    /* check for the "send a big file by hand" example case */

    if (!strcmp((const char*) in, "/leaf.jpg"))
      {
        if (strlen(LOCAL_RESOURCE_PATH) > sizeof(leaf_path) - 10)
          return -1;
        sprintf(leaf_path, "%s/leaf.jpg", LOCAL_RESOURCE_PATH);

        /* well, let's demonstrate how to send the hard way */

        p = buffer;
        pss->fd = open(leaf_path, O_RDONLY);

        if (pss->fd < 0)
          return -1;

        fstat(pss->fd, &stat_buf);

        /*
         * we will send a big jpeg file, but it could be
         * anything.  Set the Content-Type: appropriately
         * so the browser knows what to do with it.
         */

        p += sprintf((char* )p, "HTTP/1.0 200 OK\x0d\x0a"
            "Server: libwebsockets\x0d\x0a"
            "Content-Type: image/jpeg\x0d\x0a"
            "Content-Length: %u\x0d\x0a\x0d\x0a",
            (unsigned int )stat_buf.st_size);

        /*
         * send the http headers...
         * this won't block since it's the first payload sent
         * on the connection since it was established
         * (too small for partial)
         */

        n = libwebsocket_write(wsi, buffer, p - buffer, LWS_WRITE_HTTP);

        if (n < 0)
          {
            close(pss->fd);
            return -1;
          }
        /*
         * book us a LWS_CALLBACK_HTTP_WRITEABLE callback
         */
        libwebsocket_callback_on_writable(context, wsi);
        break;
      }

    /* if not, send a file the easy way */

    for (n = 0; n < (sizeof(whitelist) / sizeof(whitelist[0]) - 1); n++)
      if (in && strcmp((const char*) in, whitelist[n].urlpath) == 0)
        break;

    sprintf(buf, "%s%s", LOCAL_RESOURCE_PATH, whitelist[n].urlpath);

    if (libwebsockets_serve_http_file(context, wsi, buf, whitelist[n].mimetype))
      return -1; /* through completion or error, close the socket */

    /*
     * notice that the sending of the file completes asynchronously,
     * we'll get a LWS_CALLBACK_HTTP_FILE_COMPLETION callback when
     * it's done
     */

    break;

  case LWS_CALLBACK_HTTP_FILE_COMPLETION:
//		lwsl_info("LWS_CALLBACK_HTTP_FILE_COMPLETION seen\n");
    /* kill the connection after we sent one file */
    return -1;

  case LWS_CALLBACK_HTTP_WRITEABLE:
    /*
     * we can send more of whatever it is we were sending
     */

    do
      {
        n = read(pss->fd, buffer, sizeof buffer);
        /* problem reading, close conn */
        if (n < 0)
          goto bail;
        /* sent it all, close conn */
        if (n == 0)
          goto bail;
        /*
         * because it's HTTP and not websocket, don't need to take
         * care about pre and postamble
         */
        m = libwebsocket_write(wsi, buffer, n, LWS_WRITE_HTTP);
        if (m < 0)
          /* write failed, close conn */
          goto bail;
        if (m != n)
          /* partial write, adjust */
          lseek(pss->fd, m - n, SEEK_CUR);

      }
    while (!lws_send_pipe_choked(wsi));
    libwebsocket_callback_on_writable(context, wsi);
    break;

    bail: close(pss->fd);
    return -1;

    /*
     * callback for confirming to continue with client IP appear in
     * protocol 0 callback since no websocket protocol has been agreed
     * yet.  You can just ignore this if you won't filter on client IP
     * since the default uhandled callback return is 0 meaning let the
     * connection continue.
     */

  case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
#if 0
    libwebsockets_get_peer_addresses(context, wsi, (int)(long)in, client_name,
        sizeof(client_name), client_ip, sizeof(client_ip));

    fprintf(stderr, "Received network connect from %s (%s)\n",
        client_name, client_ip);
#endif
    /* if we returned non-zero from here, we kill the connection */
    break;

#ifdef EXTERNAL_POLL
    /*
     * callbacks for managing the external poll() array appear in
     * protocol 0 callback
     */

    case LWS_CALLBACK_ADD_POLL_FD:

    if (count_pollfds >= max_poll_elements)
      {
        lwsl_err("LWS_CALLBACK_ADD_POLL_FD: too many sockets to track\n");
        return 1;
      }

    fd_lookup[fd] = count_pollfds;
    pollfds[count_pollfds].fd = fd;
    pollfds[count_pollfds].events = (int)(long)len;
    pollfds[count_pollfds++].revents = 0;
    break;

    case LWS_CALLBACK_DEL_POLL_FD:
    if (!--count_pollfds)
    break;
    m = fd_lookup[fd];
    /* have the last guy take up the vacant slot */
    pollfds[m] = pollfds[count_pollfds];
    fd_lookup[pollfds[count_pollfds].fd] = m;
    break;

    case LWS_CALLBACK_SET_MODE_POLL_FD:
    pollfds[fd_lookup[fd]].events |= (int)(long)len;
    break;

    case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
    pollfds[fd_lookup[fd]].events &= ~(int)(long)len;
    break;
#endif

  default:
    break;
    }

  return 0;
}

/*
 * this is just an example of parsing handshake headers, you don't need this
 * in your code unless you will filter allowing connections by the header
 * content
 */

static void
dump_handshake_info(struct libwebsocket *wsi)
{
  int n;
  static const char *token_names[WSI_TOKEN_COUNT] =
    {
    /*[WSI_TOKEN_GET_URI]		=*/"GET URI",
    /*[WSI_TOKEN_HOST]		=*/"Host",
    /*[WSI_TOKEN_CONNECTION]	=*/"Connection",
    /*[WSI_TOKEN_KEY1]		=*/"key 1",
    /*[WSI_TOKEN_KEY2]		=*/"key 2",
    /*[WSI_TOKEN_PROTOCOL]		=*/"Protocol",
    /*[WSI_TOKEN_UPGRADE]		=*/"Upgrade",
    /*[WSI_TOKEN_ORIGIN]		=*/"Origin",
    /*[WSI_TOKEN_DRAFT]		=*/"Draft",
    /*[WSI_TOKEN_CHALLENGE]		=*/"Challenge",

    /* new for 04 */
    /*[WSI_TOKEN_KEY]		=*/"Key",
    /*[WSI_TOKEN_VERSION]		=*/"Version",
    /*[WSI_TOKEN_SWORIGIN]		=*/"Sworigin",

    /* new for 05 */
    /*[WSI_TOKEN_EXTENSIONS]	=*/"Extensions",

    /* client receives these */
    /*[WSI_TOKEN_ACCEPT]		=*/"Accept",
    /*[WSI_TOKEN_NONCE]		=*/"Nonce",
    /*[WSI_TOKEN_HTTP]		=*/"Http",
    /*[WSI_TOKEN_MUXURL]	=*/"MuxURL", };
  char buf[256];

  for (n = 0; n < WSI_TOKEN_COUNT; n++)
    {
      if (!lws_hdr_total_length(wsi, n))
        continue;

      lws_hdr_copy(wsi, buf, sizeof buf, n);

      fprintf(stderr, "    %s = %s\n", token_names[n], buf);
    }
}

/* MQTT bridge protocol */

static void
ws_subscribe(struct libwebsocket *wsi, Message *msg) {
  if (subscribed_to(&SUBSCRIPTIONS, msg->topic, wsi))
    {
      lwsl_notice("Client already subscribed to topic : %s\n", msg->topic);
    }
  else
    {
      // check whether we already subscribed to the MQTT topic
      Subscription *sub = subscription_get(&SUBSCRIPTIONS, msg->topic);

      // first add client to subscribers
      lwsl_notice("Subscribing client to topic : %s\n", msg->topic);
      if (subscribe(&SUBSCRIPTIONS, msg->topic, wsi))
        {
          lwsl_notice("Client subscribed to topic : %s\n", msg->topic);
        }

      /*
       * Subscribe to MQTT topic if not already subscribed.
       * We only subcribe once for each topic to the server.
       */
      if (sub)
          llog(LOG_NOTICE, "Already subscribed to MQTT topic : %s\n", msg->topic);
      else
        {
          llog(LOG_NOTICE, "Subscribing to MQTT topic : %s\n", msg->topic);
          mosquitto_subscribe(MOSQUITTO, NULL, msg->topic, 0);
        }
    }
}

static void
ws_unsubscribe(struct libwebsocket *wsi, Message *msg) {
  if (subscribed_to(&SUBSCRIPTIONS, msg->topic, wsi))
      unsubscribe(&SUBSCRIPTIONS, msg->topic, wsi);
  else
    lwsl_notice("Client not subscribed to topic : %s\n", msg->topic);
}

static void
ws_publish(Message *msg)
{
  mosquitto_publish(MOSQUITTO, NULL, msg->topic, strlen(msg->data), msg->data, 0, false);
}

static int
callback_lws_mqtt_bridge(struct libwebsocket_context *context,
    struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
    void *user, void *in, size_t len)
{
  lwsl_info("callback_lws_mqtt_bridge\n");
  switch (reason)
    {
  case LWS_CALLBACK_ESTABLISHED:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_ESTABLISHED\n");
    break;
  case LWS_CALLBACK_SERVER_WRITEABLE:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_SERVER_WRITEABLE\n");
    // we don't use the server callback and write to the clients directly when a message was received
    break;
  case LWS_CALLBACK_CLOSED:
    lwsl_notice("Connection closed: Unsubscribe from all subscriptions\n");
    unsubscribe_all(&SUBSCRIPTIONS, wsi);
    break;
  case LWS_CALLBACK_RECEIVE:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_RECEIVE\n");

    Message msg;
    message_parse(&msg, (char *) in);
    message_serialize(&msg);

    switch(msg.method)
    {
    case PUBLISH:
      ws_publish(&msg);
      break;
    case SUBSCRIBE:
      ws_subscribe(wsi, &msg);
      break;
    case UNSUBSCRIBE:
      ws_unsubscribe(wsi, &msg);
      break;
    }
    break;

  case LWS_CALLBACK_PROTOCOL_DESTROY:
    lwsl_notice("mqtt protocol cleaning up\n");
    break;

  case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
    dump_handshake_info(wsi);
    /* you could return non-zero here and kill the connection */
    break;

  default:
    lwsl_notice("callback_lws_mqtt_bridge: %d\n", reason);
    break;
    }

  return 0;
}

/* list of supported protocols and callbacks */

struct libwebsocket_protocols protocols[] =
  {
  /* first protocol must always be HTTP handler */

    { "http-only", /* name */
    callback_http, /* callback */
    sizeof(struct per_session_data__http), /* per_session_data_size */
    0, /* max frame size / rx buffer */
    },
    { "mqtt-bridge-protocol", callback_lws_mqtt_bridge,
        0, 128, },
    { NULL, NULL, 0, 0 } /* terminator */
  };

void
sighandler(int sig)
{
  force_exit = 1;
}

struct libwebsocket_context *
initialize_lws_context(struct lws_context_creation_info *info)
{
  llog(LOG_INFO, "Initialize libwebsocket context\n");
  memset(info, 0, sizeof(*info));

  info->port = 8080;
  info->protocols = protocols;
  info->extensions = libwebsocket_get_internal_extensions();
  info->ssl_cert_filepath = NULL;
  info->ssl_private_key_filepath = NULL;
  info->gid = -1;
  info->uid = -1;
  info->options = 0;

  // create libwebsocket context representing this server
  return libwebsocket_create_context(info);
}
