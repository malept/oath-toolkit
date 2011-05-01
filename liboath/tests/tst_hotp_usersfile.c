/*
 * tst_hotp_usersfile.c - self-tests for Liboath usersfile functions
 * Copyright (C) 2009, 2010, 2011 Simon Josefsson
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
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "joe", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "bob", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "670691",
				    0, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "599872",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "072768",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "foo", "755224", 0, "8989", &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }
  if (last_otp != 1260203142)
    {
      printf ("oath_authenticate_usersfile timestamp %d != 1260203142\n",
	      last_otp);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "755224", 0, "4321", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "436521", 10, "6767", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile: %d\n", rc);
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
