/*
 * test-pam_oath-root.c - self-test PAM module as root
 * Copyright (C) 2011-2012 Simon Josefsson
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <security/pam_appl.h>

static size_t loop;

/*
  Run 'oathtool 00 -w4' to generate:

  328482
  812658
  073348
  887919
  320986

  Run 'oathtool --totp --now=2006-12-07 00 -w10' to generate:

  140852
  299833
  044488
  584072
  000706
  512368
  094088
  755942
  936706
  369736
  787399
*/

/* *INDENT-OFF* */
static const struct {
  const char *user;
  const char *passwd;
  int expectrc;
} tv[] = {
  { "user1", "incorrectpassword", PAM_AUTH_ERR },
  { "user1", "328482", PAM_SUCCESS },
  { "user1", "073348", PAM_SUCCESS },
  { "user1", "812658", PAM_AUTH_ERR },
  { "user2", "bad", PAM_AUTH_ERR },
  { "user2", "328482", PAM_AUTH_ERR },
  { "user2", "pw328482", PAM_SUCCESS },
  { "user2", "pw073348", PAM_SUCCESS },
  { "user2", "pw812658", PAM_AUTH_ERR },
  { "user2", "bad887919", PAM_AUTH_ERR },
  { "user2", "pw887919", PAM_SUCCESS },
  { "user3", "bad", PAM_AUTH_ERR },
  { "user3", "299833", PAM_SUCCESS },
  { "user3", "299833", PAM_AUTH_ERR },
  { "user3", "140852", PAM_AUTH_ERR },
  { "user3", "936706", PAM_SUCCESS },
  { "user3", "512368", PAM_AUTH_ERR },
  { "user3", "787399", PAM_SUCCESS },
};
/* *INDENT-ON* */

static int
oath_conv (int num_msg, const struct pam_message **msg,
	   struct pam_response **resp, void *appdata_ptr)
{
  struct pam_response *out;

  if (num_msg != 1)
    {
      printf ("num_msg != 1?!\n");
      exit (EXIT_FAILURE);
    }

  out = malloc (sizeof (*out));
  if (out == NULL)
    return PAM_BUF_ERR;

  printf ("Got prompt (type %d): %s\n", msg[0]->msg_style, msg[0]->msg);
  out[0].resp_retcode = 0;
  out[0].resp = strdup (tv[loop].passwd);

  *resp = out;

  return PAM_SUCCESS;
}


static const struct pam_conv conv = {
  oath_conv,
  NULL
};

int
main (int argc, char **argv)
{
  pam_handle_t *pamh;
  int rc;

  for (loop = 0; loop < sizeof (tv) / sizeof (tv[0]); loop++)
    {
      rc = pam_start ("pam_oath1", tv[loop].user, &conv, &pamh);
      if (rc != PAM_SUCCESS)
	{
	  printf ("pam_start failed loop %ld rc %d: %s\n", loop, rc,
		  pam_strerror (pamh, rc));
	  return 1;
	}

      rc = pam_authenticate (pamh, 0);
      if (rc != tv[loop].expectrc)
	{
	  printf ("pam_authenticate failed loop %ld rc %d: %s\n", loop, rc,
		  pam_strerror (pamh, rc));
	  return 1;
	}

      rc = pam_end (pamh, rc);
      if (rc != PAM_SUCCESS)
	{
	  printf ("pam_end failed loop %ld rc %d: %s\n", loop, rc,
		  pam_strerror (pamh, rc));
	  return 1;
	}
    }

  return 0;
}
