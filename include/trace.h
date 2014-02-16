#include <sys/time.h>
#include <stdio.h>

#ifdef TRACE
static struct timeval _TRACE_START;
static struct timeval _TRACE_END;
static double _TRACE_DIFF;

#define TRACE_BEGIN gettimeofday(&_TRACE_START, NULL);
#define TRACE_END(message) \
  gettimeofday(&_TRACE_END, NULL); \
        _TRACE_DIFF = (_TRACE_END.tv_sec - _TRACE_START.tv_sec) * 1000.0; \
        _TRACE_DIFF += (_TRACE_END.tv_usec - _TRACE_START.tv_usec) / 1000.0; \
        printf("Trace [%s] duration: %.2f ms\n", message, _TRACE_DIFF);
#else
#define TRACE_BEGIN
#define TRACE_END
#endif

