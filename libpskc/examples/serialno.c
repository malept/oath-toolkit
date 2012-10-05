#include <stdio.h>

#include <pskc.h>

int
main (int argc, const char *argv[])
{
  char buffer[4096];
  FILE *fh = fopen (argv[1], "r");
  size_t len = fread (buffer, 1, sizeof (buffer), fh);
  pskc_t *container;
  pskc_key_t *keypackage;

  fclose (fh);

  pskc_global_init ();

  pskc_init (&container);
  pskc_parse_from_memory (container, len, buffer);

  keypackage = pskc_get_keypackage (container, 0);

  printf ("SerialNo: %s\n", pskc_get_key_id (keypackage));

  pskc_done (container);
  pskc_global_done ();
}
