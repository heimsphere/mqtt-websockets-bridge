#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <mosquitto.h>

#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/song.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>
#include <mpd/idle.h>

#include <mpd/connection.h>

#define LOG_INFO(x, ...) \
        fprintf(stderr, x "\n", __VA_ARGS__);

#define LOG_WARNING(x, ...) \
        fprintf(stderr, x "\n", __VA_ARGS__);

#define LOG_ERROR(x, ...) \
        fprintf(stderr, "(%s:%d) %s : " x "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

int
new_connection(struct mpd_connection **conn);

int
version(struct mpd_connection *conn);

void
print_status(struct mpd_status *status);
