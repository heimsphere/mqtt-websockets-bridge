/* logger interface */
#include <syslog.h> /* required for log level definitions */

void
llog_init();

void
llog(int level, const char *fmt, ...);
