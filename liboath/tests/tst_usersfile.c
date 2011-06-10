/*
 * tst_usersfile.c - self-tests for Liboath usersfile functions
 * Copyright (C) 2009-2011 Simon Josefsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <config.h>

#include "oath.h"

#include <stdio.h>

#define CREDS "tmp.oath"

int
main (void)
{
  oath_rc rc;
  time_t last_otp;

  if (!oath_check_version (OATH_VERSION))
    {
      printf ("oath_check_version (%s) failed [%s]\n", OATH_VERSION,
	      oath_check_version (NULL));
      return 1;
    }

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile ("no-such-file", "joe", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_NO_SUCH_FILE)
    {
      printf ("oath_authenticate_usersfile[1]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "joe", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[2]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "bob", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[3]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "670691",
				    0, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[4]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "599872",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[5]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "072768",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[6]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "foo", "755224", 0, "8989", &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[7]: %d\n", rc);
      return 1;
    }
  {
    struct tm tm;
    char *ts;
    time_t expected_last_otp;

    /* We need to convert local time 2009-12-07T17:25:42L into Unix
       time.  The values depends on local timezone on host.  */

    ts = strptime ("2009-12-07T17:25:42L", "%Y-%m-%dT%H:%M:%SL", &tm);
    if (ts == NULL || *ts != '\0')
      {
	printf ("strptime failed\n");
	return 1;
      }
    tm.tm_isdst = -1;
    expected_last_otp = mktime (&tm);
    if (expected_last_otp == (time_t) - 1)
      {
	printf ("mktime failed\n");
	return 1;
      }
    if (last_otp != expected_last_otp)
      {
	printf ("oath_authenticate_usersfile timestamp %d != %d\n",
		last_otp, expected_last_otp);
	return 1;
      }
  }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "755224", 0, "4321", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[8]: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "436521", 10, "6767", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[9]: %d\n", rc);
      return 1;
    }

  /*
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

  /* Test completely invalid OTP */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "386397", 0, "4711", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[10]: %d\n", rc);
      return 1;
    }

  /* Test the next OTP but search window = 0. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "299833", 0, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[11]: %d\n", rc);
      return 1;
    }

  /* Test the next OTP with search window = 1. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "299833", 1, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[12]: %d\n", rc);
      return 1;
    }

  /* Test to replay last OTP. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "299833", 1, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[13]: %d\n", rc);
      return 1;
    }

  /* Test to replay previous OTP. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "140852", 1, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[14]: %d\n", rc);
      return 1;
    }

  /* Try an OTP in the future but outside search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "369736", 1, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[15]: %d\n", rc);
      return 1;
    }

  /* Try OTP in the future with good search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "369736", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[16]: %d\n", rc);
      return 1;
    }

  /* Now try a rather old OTP within search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "000706", 10, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[17]: %d\n", rc);
      return 1;
    }

  rc = oath_done ();
  if (rc != OATH_OK)
    {
      printf ("oath_done: %d\n", rc);
      return 1;
    }

  return 0;
}
