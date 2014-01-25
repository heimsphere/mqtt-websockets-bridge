#include "bridge.h"

/*
 * libwebsockets-test-server - libwebsockets test implementation
 *
 * Copyright (C) 2010-2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
#ifdef CMAKE_BUILD
#include <lws_config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#ifdef WIN32

#ifdef EXTERNAL_POLL
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stddef.h>

#include "websock-w32.h"
#endif

#else // NOT WIN32
#include <syslog.h>
#endif

#include <signal.h>

#include <libwebsockets.h>

static int close_testing;
int max_poll_elements;

struct pollfd *pollfds;
int *fd_lookup;
int count_pollfds;
int force_exit = 0;

/*
 * This demo server shows how to use libwebsockets for one or more
 * websocket protocols in the same server
 *
 * It defines the following websocket protocols:
 *
 *  dumb-increment-protocol:  once the socket is opened, an incrementing
 *				ascii string is sent down it every 50ms.
 *				If you send "reset\n" on the websocket, then
 *				the incrementing number is reset to 0.
 *
 *  lws-mirror-protocol: copies any received packet to every connection also
 *				using this protocol, including the sender
 */

#define LOCAL_RESOURCE_PATH INSTALL_DATADIR "/libwebsockets-test-server"
char *resource_path = LOCAL_RESOURCE_PATH;

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
        if (strlen(resource_path) > sizeof(leaf_path) - 10)
          return -1;
        sprintf(leaf_path, "%s/leaf.jpg", resource_path);

        /* well, let's demonstrate how to send the hard way */

        p = buffer;

#ifdef WIN32
        pss->fd = open(leaf_path, O_RDONLY | _O_BINARY);
#else
        pss->fd = open(leaf_path, O_RDONLY);
#endif

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

    sprintf(buf, "%s%s", resource_path, whitelist[n].urlpath);

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

/* dumb_increment protocol */

/*
 * one of these is auto-created for each connection and a pointer to the
 * appropriate instance is passed to the callback in the user parameter
 *
 * for this example protocol we use it to individualize the count for each
 * connection.
 */

struct per_session_data__dumb_increment
{
  int number;
};

static int
callback_dumb_increment(struct libwebsocket_context *context,
    struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
    void *user, void *in, size_t len)
{
  int n, m;
  unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
  LWS_SEND_BUFFER_POST_PADDING];
  unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
  struct per_session_data__dumb_increment *pss =
      (struct per_session_data__dumb_increment *) user;

  switch (reason)
    {

  case LWS_CALLBACK_ESTABLISHED:
    lwsl_info("callback_dumb_increment: "
        "LWS_CALLBACK_ESTABLISHED\n");
    pss->number = 0;
    break;

  case LWS_CALLBACK_SERVER_WRITEABLE:
    n = sprintf((char* )p, "%d", pss->number++);
    m = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
    if (m < n)
      {
        lwsl_err("ERROR %d writing to di socket\n", n);
        return -1;
      }
    if (close_testing && pss->number == 50)
      {
        lwsl_info("close tesing limit, closing\n");
        return -1;
      }
    break;

  case LWS_CALLBACK_RECEIVE:
//		fprintf(stderr, "rx %d\n", (int)len);
    if (len < 6)
      break;
    if (strcmp((const char*) in, "reset\n") == 0)
      pss->number = 0;
    break;
    /*
     * this just demonstrates how to use the protocol filter. If you won't
     * study and reject connections based on header content, you don't need
     * to handle this callback
     */

  case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
    dump_handshake_info(wsi);
    /* you could return non-zero here and kill the connection */
    break;

  default:
    break;
    }

  return 0;
}

/* lws-mirror_protocol */

#define MAX_MESSAGE_QUEUE 32

struct per_session_data__lws_mirror
{
  struct libwebsocket *wsi;
  int ringbuffer_tail;
};

struct a_message
{
  void *payload;
  size_t len;
};

static struct a_message ringbuffer[MAX_MESSAGE_QUEUE];
static int ringbuffer_head;

static int
callback_lws_mirror(struct libwebsocket_context *context,
    struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
    void *user, void *in, size_t len)
{
  int n;
  struct per_session_data__lws_mirror *pss =
      (struct per_session_data__lws_mirror *) user;

  switch (reason)
    {

  case LWS_CALLBACK_ESTABLISHED:
    lwsl_info("callback_lws_mirror: LWS_CALLBACK_ESTABLISHED\n");
    pss->ringbuffer_tail = ringbuffer_head;
    pss->wsi = wsi;
    break;

  case LWS_CALLBACK_PROTOCOL_DESTROY:
    lwsl_notice("mirror protocol cleaning up\n");
    for (n = 0; n < sizeof ringbuffer / sizeof ringbuffer[0]; n++)
      if (ringbuffer[n].payload)
        free(ringbuffer[n].payload);
    break;

  case LWS_CALLBACK_SERVER_WRITEABLE:
    if (close_testing)
      break;
    while (pss->ringbuffer_tail != ringbuffer_head)
      {

        n = libwebsocket_write(wsi,
            (unsigned char*) ringbuffer[pss->ringbuffer_tail].payload +
            LWS_SEND_BUFFER_PRE_PADDING, ringbuffer[pss->ringbuffer_tail].len,
            LWS_WRITE_TEXT);
        if (n < ringbuffer[pss->ringbuffer_tail].len)
          {
            lwsl_err("ERROR %d writing to mirror socket\n", n);
            return -1;
          }
        if (n < ringbuffer[pss->ringbuffer_tail].len)
          lwsl_err("mirror partial write %d vs %d\n", n,
              ringbuffer[pss->ringbuffer_tail].len);

        if (pss->ringbuffer_tail == (MAX_MESSAGE_QUEUE - 1))
          pss->ringbuffer_tail = 0;
        else
          pss->ringbuffer_tail++;

        if (((ringbuffer_head - pss->ringbuffer_tail) & (MAX_MESSAGE_QUEUE - 1))
            == (MAX_MESSAGE_QUEUE - 15))
          libwebsocket_rx_flow_allow_all_protocol(
              libwebsockets_get_protocol(wsi));

        // lwsl_debug("tx fifo %d\n", (ringbuffer_head - pss->ringbuffer_tail) & (MAX_MESSAGE_QUEUE - 1));

        if (lws_send_pipe_choked(wsi))
          {
            libwebsocket_callback_on_writable(context, wsi);
            break;
          }
        /*
         * for tests with chrome on same machine as client and
         * server, this is needed to stop chrome choking
         */
        usleep(1);
      }
    break;

  case LWS_CALLBACK_RECEIVE:

    if (((ringbuffer_head - pss->ringbuffer_tail) & (MAX_MESSAGE_QUEUE - 1))
        == (MAX_MESSAGE_QUEUE - 1))
      {
        lwsl_err("dropping!\n");
        goto choke;
      }

    if (ringbuffer[ringbuffer_head].payload)
      free(ringbuffer[ringbuffer_head].payload);

    ringbuffer[ringbuffer_head].payload = malloc(
    LWS_SEND_BUFFER_PRE_PADDING + len +
    LWS_SEND_BUFFER_POST_PADDING);
    ringbuffer[ringbuffer_head].len = len;
    memcpy(
        (char*)ringbuffer[ringbuffer_head].payload + LWS_SEND_BUFFER_PRE_PADDING,
        in, len);
    if (ringbuffer_head == (MAX_MESSAGE_QUEUE - 1))
      ringbuffer_head = 0;
    else
      ringbuffer_head++;

    if (((ringbuffer_head - pss->ringbuffer_tail) & (MAX_MESSAGE_QUEUE - 1))
        != (MAX_MESSAGE_QUEUE - 2))
      goto done;

    choke: lwsl_debug("LWS_CALLBACK_RECEIVE: throttling %p\n", wsi);
    libwebsocket_rx_flow_control(wsi, 0);

//		lwsl_debug("rx fifo %d\n", (ringbuffer_head - pss->ringbuffer_tail) & (MAX_MESSAGE_QUEUE - 1));
    done: libwebsocket_callback_on_writable_all_protocol(
        libwebsockets_get_protocol(wsi));
    break;

    /*
     * this just demonstrates how to use the protocol filter. If you won't
     * study and reject connections based on header content, you don't need
     * to handle this callback
     */

  case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
    dump_handshake_info(wsi);
    /* you could return non-zero here and kill the connection */
    break;

  default:
    break;
    }

  return 0;
}

/* MQTT bridge protocol */

struct per_session_data__mqtt_bridge
{
  char *data;
  size_t len;
};

static int
callback_lws_mqtt_bridge(struct libwebsocket_context *context,
    struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
    void *user, void *in, size_t len)
{
  unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512
      + LWS_SEND_BUFFER_POST_PADDING];
  unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
  struct per_session_data__mqtt_bridge *pss =
      (struct per_session_data__mqtt_bridge *) user;

  lwsl_info("callback_lws_mqtt_bridge\n");
  switch (reason)
    {
  case LWS_CALLBACK_ESTABLISHED:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_ESTABLISHED\n");
    break;
  case LWS_CALLBACK_SERVER_WRITEABLE:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_SERVER_WRITEABLE\n");
//		n = sprintf((char *)p, "%s", "foobar");
//		m = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
//		if (m < n) {
//			lwsl_err("ERROR %d writing to di socket\n", n);
//			return -1;
//		}
    break;
  case LWS_CALLBACK_RECEIVE:
    lwsl_notice("callback_lws_mqtt_bridge: LWS_CALLBACK_RECEIVE\n");
    lwsl_notice("rx length %d\n", (int )len);
    lwsl_notice("data: %s\n", (char* )in);

    /* subscribe to the message */
    // TODO remember the the subsciption so we can dispatch messages to the client
    if (subscribe(&SUBSCRIPTIONS, (char *) in, wsi))
      {
        lwsl_notice("Client subscribed to topic %s\n", (char* ) in);
        // FIXME don't subscribe again if already subscribed
        mosquitto_subscribe(MOSQUITTO, NULL, (char*) in, 0);
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
    { "dumb-increment-protocol", callback_dumb_increment,
        sizeof(struct per_session_data__dumb_increment), 10, },
    { "lws-mirror-protocol", callback_lws_mirror,
        sizeof(struct per_session_data__lws_mirror), 128, },
    { "mqtt-bridge-protocol", callback_lws_mqtt_bridge,
        sizeof(struct per_session_data__mqtt_bridge), 10, },
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
