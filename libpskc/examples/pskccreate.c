#include <stdio.h>
#include <pskc/pskc.h>

/*
 * $ cc -o pskccreate pskccreate.c $(pkg-config --cflags --libs libpskc)
 * $ ./pskccreate
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
  size_t len;
  pskc_t *container;
  pskc_key_t *keypackage;
  char *out;
  int rc;

  rc = pskc_global_init (); PSKC_CHECK_RC;
  rc = pskc_init (&container); PSKC_CHECK_RC;

  rc = pskc_add_keypackage (container, &keypackage); PSKC_CHECK_RC;

  pskc_set_device_manufacturer (keypackage, "Acme");
  pskc_set_device_serialno (keypackage, "42");

  pskc_set_key_id (keypackage, "4711");
  pskc_set_key_algorithm (keypackage, "urn:ietf:params:xml:ns:keyprov:pskc:hotp");

  pskc_set_key_algparm_resp_encoding (keypackage, PSKC_VALUEFORMAT_DECIMAL);
  pskc_set_key_algparm_resp_length (keypackage, 8);

  pskc_set_key_data_counter (keypackage, 42);

  rc = pskc_set_key_data_b64secret (keypackage, "Zm9v"); PSKC_CHECK_RC;

  rc = pskc_build_xml (container, &out, &len); PSKC_CHECK_RC;
  fwrite (out, 1, len, stdout);
  pskc_free (out);

  pskc_done (container);
  pskc_global_done ();

  return 0;
}
