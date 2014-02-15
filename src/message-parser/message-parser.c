#include "message.h"

#define BUFFER_SIZE 4096

int
main(int argc, char **argv)
{
  FILE *f = fopen(argv[1], "r");

  char buf[BUFFER_SIZE];
  size_t size = fread(buf, sizeof(char), BUFFER_SIZE, f);

  if (size <= 0)
    {
      printf("Error: no input!\n");
      exit(-1);
    }

  if (size != BUFFER_SIZE && !feof(f))
    {
      printf("ERROR: %s\n", strerror(ferror(f)));
      fclose(f);
      exit(-1);
    }

  fclose(f);

  Message message;
  Message_parse(&message, buf, size);
  Envelope_fwrite(&(message.envelope), stdout);
  printf("-----\n");
  if (message.body)
    printf("%s\n", message.body);
  printf("-----\n");
  Message_free(&message);
}
