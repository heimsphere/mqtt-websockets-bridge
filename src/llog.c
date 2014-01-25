#include <syslog.h>
#include <stdarg.h>     /* va_ */
#include <assert.h>
#include <stdio.h>      /* vprintf */

/*
 * watches a unix domain socket and mosquitto input/output
 * sends messages for each line on the file descriptor
 * echoes the line to the socket
 * watch files -> use inotify http://www.thegeekstuff.com/2010/04/inotify-c-program-example/
 */

void
llog(int level, const char *fmt, ...)
{
  va_list argptr;

  va_start(argptr, fmt);
//     vsyslog(level, fmt, argptr);
  vprintf(fmt, argptr);
}

void
llog_init()
{
  ; /* NOOP */
}
