#include <stdio.h>
#include <pskc/pskc.h>

/*
 * $ cc -o serialno serialno.c $(pkg-config --cflags --libs libpskc)
 * $ ./serialno pskc-hotp.xml
 * SerialNo: 987654321
 * $
 */

#define PSKC_CHECK_RC					   \
  if (rc != PSKC_OK) {					   \
    printf ("%s (%d): %s\n", pskc_strerror_name (rc),	   \
	    rc, pskc_strerror (rc));			   \
    return 1;						   \
  }

int
main (int argc, const char *argv[])
{
  char buffer[4096];
  FILE *fh = fopen (argv[1], "r");
  size_t len = fread (buffer, 1, sizeof (buffer), fh);
  pskc_t *container;
  pskc_key_t *keypackage;
  int rc;

  fclose (fh);

  rc = pskc_global_init ();
  PSKC_CHECK_RC;

  rc = pskc_init (&container);
  PSKC_CHECK_RC;
  rc = pskc_parse_from_memory (container, len, buffer);
  PSKC_CHECK_RC;

  keypackage = pskc_get_keypackage (container, 0);

  if (keypackage)
    printf ("SerialNo: %s\n", pskc_get_device_serialno (keypackage));

  pskc_done (container);
  pskc_global_done ();
}
