#include "mpd-mqtt-client.h"
#include <pthread.h>

static int
connect(struct mpd_connection **conn)
{
	*conn = mpd_connection_new(NULL, 0, 30000);
	if (*conn == NULL)
	{
		LOG_ERROR("%s", "Out of memory");
		return -1;
	}

	if (mpd_connection_get_error(*conn) != MPD_ERROR_SUCCESS)
	{
		LOG_ERROR("%s", mpd_connection_get_error_message(*conn));
		mpd_connection_free(*conn);
		*conn = NULL;
		return -1;
	}
	return 1;
}

static int volume = 0;

void
changeVolume(void *val)
{

	struct mpd_connection *con = (struct mpd_connection *)val;

	printf("Change volume to %d on connection : %p\n", volume, con);
	sleep(1);

	if (!mpd_run_set_volume(con, volume++))
		printf("Failed to change volume\n");
}

/*
 * start two thread, one listening to the mosquitto queue and executing commands
 * another listening on the idle loop sending status updates
 */

int
main(int argc, char**argv)
{
	struct mpd_connection *idle_con;
	struct mpd_connection *cmd_con;

	connect(&idle_con);
	connect(&cmd_con);

	pthread_t t;

	for (;; )
	{
//      pthread_create(&t, NULL, &changeVolume, (void *) cmd_con);
		printf("Enter idle loop\n");
		enum mpd_idle event = mpd_run_idle(idle_con);
		printf("idle event : %d\n", event);
	}
	mpd_connection_free(idle_con);
	mpd_connection_free(cmd_con);
	idle_con = NULL;
	cmd_con = NULL;

	return 0;
}
