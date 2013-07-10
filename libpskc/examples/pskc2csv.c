#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pskc/pskc.h>

/*
 * $ cc -o pskc2csv pskc2csv.c $(pkg-config --cflags --libs libpskc)
 * $ ./pskc2csv pskc.xml 2> /dev/null
 * 12345678,12345678,MTIzNDU2Nzg5MDEyMzQ1Njc4OTA=
 * $
 */

int
main (int argc, const char *argv[])
{
  struct stat st;
  FILE *fh = NULL;
  char *buffer = NULL, *out;
  size_t i;
  pskc_t *container = NULL;
  pskc_key_t *keypackage;
  int exit_code = EXIT_FAILURE, rc, isvalid;

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      fprintf (stderr, "pskc_global_init: %s\n", pskc_strerror (rc));
      goto done;
    }

  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s PSKCFILE\n", argv[0]);
      goto done;
    }

  /* Part 1: Read file. */

  fh = fopen (argv[1], "r");
  if (fh == NULL)
    {
      perror ("fopen");
      goto done;
    }

  if (fstat (fileno (fh), &st) != 0)
    {
      perror ("fstat");
      goto done;
    }

  buffer = malloc (st.st_size);
  if (buffer == NULL)
    {
      perror ("malloc");
      goto done;
    }

  i = fread (buffer, 1, st.st_size, fh);
  if (i != st.st_size)
    {
      fprintf (stderr, "short read\n");
      goto done;
    }

  /* Part 2: Parse PSKC data. */

  rc = pskc_init (&container);
  if (rc != PSKC_OK)
    {
      fprintf (stderr, "pskc_init: %s\n", pskc_strerror (rc));
      goto done;
    }

  rc = pskc_parse_from_memory (container, i, buffer);
  if (rc != PSKC_OK)
    {
      fprintf (stderr, "pskc_parse_from_memory: %s\n", pskc_strerror (rc));
      goto done;
    }

  /* Part 3: Output human readable variant of PSKC data to stderr. */

  rc = pskc_output (container, PSKC_OUTPUT_HUMAN_COMPLETE, &out, &i);
  if (rc != PSKC_OK)
    {
      fprintf (stderr, "pskc_output: %s\n", pskc_strerror (rc));
      goto done;
    }

  fprintf (stderr, "%.*s\n", (int) i, out);

  pskc_free (out);

  /* Part 4: Validate PSKC data. */

  rc = pskc_validate (container, &isvalid);
  if (rc != PSKC_OK)
    {
      fprintf (stderr, "pskc_validate: %s\n", pskc_strerror (rc));
      goto done;
    }

  fprintf (stderr, "PSKC data is Schema valid: %s\n", isvalid ? "YES" : "NO");

  /* Part 5: Iterate through keypackages and print key id, device
     serial number and base64 encoded secret. */

  for (i = 0; (keypackage = pskc_get_keypackage (container, i)); i++)
    {
      const char *key_id = pskc_get_key_id (keypackage);
      const char *device_serialno = pskc_get_key_id (keypackage);
      const char *b64secret = pskc_get_key_data_b64secret (keypackage);

      printf ("%s,%s,%s\n", key_id ? key_id : "",
	      device_serialno ? device_serialno : "",
	      b64secret ? b64secret : "");
    }

  exit_code = EXIT_SUCCESS;

done:
  pskc_done (container);
  if (fh && fclose (fh) != 0)
    perror ("fclose");
  free (buffer);
  pskc_global_done ();
  exit (exit_code);
}
