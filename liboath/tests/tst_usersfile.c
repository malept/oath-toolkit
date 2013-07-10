/*
 * tst_usersfile.c - self-tests for Liboath usersfile functions
 * Copyright (C) 2009-2013 Simon Josefsson
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

#include <sys/stat.h>

#define CREDS "tmp.oath"

int
main (void)
{
  oath_rc rc;
  time_t last_otp;
  struct stat ufstat1;
  struct stat ufstat2;

  if (!oath_check_version (OATH_VERSION))
    {
      printf ("oath_check_version (%s) failed [%s]\n", OATH_VERSION,
	      oath_check_version (NULL));
      return 1;
    }

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %s (%d)\n", oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile ("no-such-file", "joe", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_NO_SUCH_FILE)
    {
      printf ("oath_authenticate_usersfile[1]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Record the current usersfile inode */
  stat (CREDS, &ufstat1);

  rc = oath_authenticate_usersfile (CREDS, "joe", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[2]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Check that we do not update usersfile on not OATH_OK */
  stat (CREDS, &ufstat2);
  if (ufstat1.st_ino != ufstat2.st_ino)
    {
      printf ("oath_authenticate_usersfile[26]: usersfile %s changed "
	      "on OATH_BAD_PASSWORD\n", CREDS);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "bob", "755224",
				    0, "1234", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[3]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "670691",
				    0, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[4]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  stat (CREDS, &ufstat2);
  if (ufstat1.st_ino == ufstat2.st_ino)
    {
      printf ("oath_authenticate_usersfile[27]: usersfile %s did not "
	      "change on OATH_OK\n", CREDS);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "599872",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[5]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS, "silver", "072768",
				    1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[6]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  stat (CREDS, &ufstat1);
  rc = oath_authenticate_usersfile (CREDS,
				    "foo", "755224", 0, "8989", &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[7]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }
  if (last_otp != 1260206742)
    {
      printf ("oath_authenticate_usersfile timestamp %ld != 1260203142\n",
	      last_otp);
      return 1;
    }

  stat (CREDS, &ufstat2);
  if (ufstat1.st_ino != ufstat2.st_ino)
    {
      printf ("oath_authenticate_usersfile[28]: usersfile %s changed "
	      "on OATH_REPLAYED_OTP\n", CREDS);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "755224", 0, "4321", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[8]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "rms", "436521", 10, "6767", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[9]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /*
     Run 'TZ=UTC oathtool --totp --now=2006-12-07 00 -w10' to generate:

     963013
     068866
     734019
     038980
     630208
     533058
     042289
     046988
     047407
     892423
     619507
   */

  /* Test completely invalid OTP */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "386397", 0, "4711", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[10]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test the next OTP but search window = 0. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "068866", 0, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[11]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test the next OTP with search window = 1. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "068866", 1, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[12]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test to replay last OTP. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "068866", 1, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[13]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test to replay previous OTP. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "963013", 1, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[14]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try an OTP in the future but outside search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "892423", 1, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[15]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP in the future with good search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "892423", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[16]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Now try a rather old OTP within search window. */
  rc = oath_authenticate_usersfile (CREDS,
				    "eve", "630208", 10, NULL, &last_otp);
  if (rc != OATH_REPLAYED_OTP)
    {
      printf ("oath_authenticate_usersfile[17]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP that matches user's second line. */
  rc = oath_authenticate_usersfile (CREDS, "twouser",
				    "874680", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[18]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP that matches user's third and final line. */
  rc = oath_authenticate_usersfile (CREDS, "threeuser",
				    "255509", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[19]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP that matches user's third and next-to-last line. */
  rc = oath_authenticate_usersfile (CREDS, "fouruser",
				    "663447", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[19]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try incorrect OTP for user with five lines. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "812658", 10, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[20]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP that matches user's second line. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "123001", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[21]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try OTP that matches user's fourth line. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "893841", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[22]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try another OTP that matches user's second line. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "746888", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[23]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try another OTP that matches user's fifth line. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "730790", 10, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[24]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Try too old OTP for user's second line. */
  rc = oath_authenticate_usersfile (CREDS, "fiveuser",
				    "692901", 10, NULL, &last_otp);
  if (rc != OATH_INVALID_OTP)
    {
      printf ("oath_authenticate_usersfile[25]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test password field of + */
  rc = oath_authenticate_usersfile (CREDS,
				    "plus", "328482", 1, "4711", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[26]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "plus", "812658", 1, "4712", &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[27]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Test different tokens with different passwords for one user */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "898463", 5, NULL, &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[28]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "password", "989803", 5, "test",
				    &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[29]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_authenticate_usersfile (CREDS,
				    "password", "427517", 5, "darn",
				    &last_otp);
  if (rc != OATH_OK)
    {
      printf ("oath_authenticate_usersfile[30]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Valid OTP for first token but incorrect password. */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "917625", 5, "nope",
				    &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[31]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Valid OTP for second token but incorrect password. */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "459145", 5, "nope",
				    &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[32]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Valid OTP for first token but with password for second user. */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "917625", 5, "test",
				    &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[33]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Valid OTP for second token but with password for first user. */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "459145", 5, "", &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[34]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  /* Valid OTP for third token but with password for second user. */
  rc = oath_authenticate_usersfile (CREDS,
				    "password", "633070", 9, "test",
				    &last_otp);
  if (rc != OATH_BAD_PASSWORD)
    {
      printf ("oath_authenticate_usersfile[35]: %s (%d)\n",
	      oath_strerror_name (rc), rc);
      return 1;
    }

  rc = oath_done ();
  if (rc != OATH_OK)
    {
      printf ("oath_done: %s (%d)\n", oath_strerror_name (rc), rc);
      return 1;
    }

  return 0;
}
