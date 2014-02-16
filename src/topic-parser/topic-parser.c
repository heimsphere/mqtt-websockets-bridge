#include "topic.h"

int
main(int argc, char **argv)
{
	FILE *f = fopen(argv[1], "r");

	char buf[1024];
	int len = fread(buf, sizeof(char), 1024, f);

	if (!feof(f))
	{
		printf("ERROR: %s\n", strerror(ferror(f)));
		fclose(f);
		exit(-1);
	}
	fclose(f);

	ParsedTopic topic;
	ParsedTopic_parse(&topic, buf, len);
	ParsedTopic_pretty_print(&topic);
	ParsedTopic_free(&topic);
}
