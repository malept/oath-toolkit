#include <stdio.h>
#include <stdlib.h>

#include <security/pam_appl.h>

int
main (int argc, char **argv)
{
  pam_handle_t *pamh = NULL;
  int rc;

  rc = pam_sm_authenticate (pamh, 0, 1, argv);

  printf ("rc %d\n", rc);

  return 0;
}
