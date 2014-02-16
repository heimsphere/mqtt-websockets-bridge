#include <syslog.h>
#include <stdarg.h>     /* va_ */
#include <assert.h>
#include <stdio.h>      /* vprintf */

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
