#include "mpd-mqtt-client.h"

int
new_connection(struct mpd_connection **conn)
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
	return 0;
}

int
version(struct mpd_connection *conn)
{
	int i, total = -1;

	for (i = 0; i < 3; ++i)
	{
		LOG_INFO("version[%i]: %i", i, mpd_connection_get_server_version(conn)[i]);
		total += mpd_connection_get_server_version(conn)[i];
	}
	/* Check if at least one of the three number is positive */
	return total;
}

void
print_status(struct mpd_status *status)
{
	const struct mpd_audio_format *audio_format;

	LOG_INFO("volume: %i", mpd_status_get_volume(status));
	LOG_INFO("repeat: %i", mpd_status_get_repeat(status));
	LOG_INFO("single: %i", mpd_status_get_single(status));
	LOG_INFO("consume: %i", mpd_status_get_consume(status));
	LOG_INFO("random: %i", mpd_status_get_random(status));
	LOG_INFO("queue version: %u", mpd_status_get_queue_version(status));
	LOG_INFO("queue length: %i", mpd_status_get_queue_length(status));

	if (mpd_status_get_state(status) == MPD_STATE_PLAY
	    || mpd_status_get_state(status) == MPD_STATE_PAUSE)
	{
		LOG_INFO("song: %i", mpd_status_get_song_pos(status));
		LOG_INFO("elaspedTime: %i", mpd_status_get_elapsed_time(status));
		LOG_INFO("elasped_ms: %u\n", mpd_status_get_elapsed_ms(status));
		LOG_INFO("totalTime: %i", mpd_status_get_total_time(status));
		LOG_INFO("bitRate: %i", mpd_status_get_kbit_rate(status));
	}

	audio_format = mpd_status_get_audio_format(status);
	if (audio_format != NULL)
	{
		printf("sampleRate: %i\n", audio_format->sample_rate);
		printf("bits: %i\n", audio_format->bits);
		printf("channels: %i\n", audio_format->channels);
	}
}

void
print_tag(const struct mpd_song *song, enum mpd_tag_type type,
	  const char *label)
{
	unsigned i = 0;
	const char *value;

	while ((value = mpd_song_get_tag(song, type, i++)) != NULL)
		LOG_INFO("%s: %s", label, value);
}

void
print_song(const struct mpd_song *song)
{
	LOG_INFO("uri: %s\n", mpd_song_get_uri(song));
	print_tag(song, MPD_TAG_ARTIST, "artist");
	print_tag(song, MPD_TAG_ALBUM, "album");
	print_tag(song, MPD_TAG_TITLE, "title");
	print_tag(song, MPD_TAG_TRACK, "track");
	print_tag(song, MPD_TAG_NAME, "name");
	print_tag(song, MPD_TAG_DATE, "date");

	if (mpd_song_get_duration(song) > 0)
		LOG_INFO("time: %i", mpd_song_get_duration(song));

	LOG_INFO("pos: %u", mpd_song_get_pos(song));
}

int
status(struct mpd_connection *conn)
{
	struct mpd_status *status;

	status = mpd_run_status(conn);
	if (!status)
	{
		LOG_ERROR("%s", mpd_connection_get_error_message(conn));
		return -1;
	}

	print_status(status);
	mpd_status_free(status);

	mpd_response_finish(conn);

	return 0;
}

int
currentsong(struct mpd_connection *conn)
{
	struct mpd_song *song;

	song = mpd_run_current_song(conn);
	if (song != NULL)
	{
		print_song(song);

		mpd_song_free(song);
	}

	mpd_response_finish(conn);

	return 0;
}

int
list_status_currentsong(struct mpd_connection *conn)
{
	struct mpd_status *status;
	const struct mpd_song *song;
	struct mpd_entity *entity;

	mpd_command_list_begin(conn, true);
	mpd_send_status(conn);
	mpd_send_current_song(conn);
	mpd_command_list_end(conn);

	status = mpd_recv_status(conn);
	if (!status)
	{
		LOG_ERROR("%s", mpd_connection_get_error_message(conn));
		return -1;
	}
	if (mpd_status_get_error(status))
		LOG_WARNING("status error: %s", mpd_status_get_error(status));

	print_status(status);
	mpd_status_free(status);

	mpd_response_next(conn);

	entity = mpd_recv_entity(conn);
	if (entity)
	{
		if (mpd_entity_get_type(entity) != MPD_ENTITY_TYPE_SONG)
		{
			LOG_ERROR("entity doesn't have the expected type (song)i :%d",
				  mpd_entity_get_type(entity));
			mpd_entity_free(entity);
			return -1;
		}

		song = mpd_entity_get_song(entity);

		print_song(song);

		mpd_entity_free(entity);
	}

	mpd_response_finish(conn);

	return 0;
}

int
lsinfo(struct mpd_connection *conn, const char *path)
{
	struct mpd_entity *entity;

	mpd_send_list_meta(conn, path);

	while ((entity = mpd_recv_entity(conn)) != NULL)
	{
		const struct mpd_song *song;
		const struct mpd_directory *dir;
		const struct mpd_playlist *pl;

		switch (mpd_entity_get_type(entity))
		{
		case MPD_ENTITY_TYPE_UNKNOWN:
			printf("Unknown type\n");
			break;

		case MPD_ENTITY_TYPE_SONG:
			song = mpd_entity_get_song(entity);
			print_song(song);
			break;

		case MPD_ENTITY_TYPE_DIRECTORY:
			dir = mpd_entity_get_directory(entity);
			printf("directory: %s\n", mpd_directory_get_path(dir));
			break;

		case MPD_ENTITY_TYPE_PLAYLIST:
			pl = mpd_entity_get_playlist(entity);
			LOG_INFO("playlist: %s", mpd_playlist_get_path(pl))
			;
			break;
		}

		mpd_entity_free(entity);
	}

	mpd_response_finish(conn);

	return 0;
}

int
list_artists(struct mpd_connection *conn)
{
	struct mpd_pair *pair;
	int first = 1;

	mpd_search_db_tags(conn, MPD_TAG_ARTIST);
	mpd_search_commit(conn);

	LOG_INFO("%s: ", "Artists list");
	while ((pair = mpd_recv_pair_tag(conn, MPD_TAG_ARTIST)) != NULL)
	{
		if (first)
		{
			printf("    %s", pair->value);
			first = 0;
		}
		else
			printf(", %s", pair->value);
		mpd_return_pair(conn, pair);
	}
	printf("\n");

	mpd_response_finish(conn);
	return 0;
}
