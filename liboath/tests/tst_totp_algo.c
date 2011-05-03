/*
 * tst_totp_algo.c - self-tests for liboath TOTP algorithm functions
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

/* From draft-mraihi-totp-timebased-07 */
const struct {
  time_t secs;
  uint64_t T;
  char *otp;
} tv[] = {
  { 59, 0x0000000000000001, "94287082" },
  { 1111111109, 0x00000000023523EC, "07081804" },
  { 1111111111, 0x00000000023523ED, "14050471" },
  { 1234567890, 0x000000000273EF07, "89005924" },
  { 2000000000, 0x0000000003F940AA, "69279037" },
  { 20000000000, 0x0000000027BC86AA, "65353130" }
};

int
main (void)
{
  oath_rc rc;
  char secret[20] = "\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30"
    "\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30";
  size_t secretlen = sizeof (secret);
  char otp[10];
  size_t i;

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  if (OATH_TOTP_DEFAULT_TIME_STEP_SIZE != 30)
    {
      printf ("strange OATH_TOTP_DEFAULT_TIME_STEP_SIZE value %d?!\n",
	      OATH_TOTP_DEFAULT_TIME_STEP_SIZE);
      return 1;
    }

  if (OATH_TOTP_DEFAULT_START_TIME != 0)
    {
      printf ("strange OATH_TOTP_DEFAULT_START_TIME value %ld?!\n",
	      (long) OATH_TOTP_DEFAULT_START_TIME);
      return 1;
    }

  for (i = 0; i < sizeof (tv) / sizeof (tv[0]); i++)
    {
      rc = oath_totp_generate (secret, secretlen, tv[i].secs,
			       OATH_TOTP_DEFAULT_TIME_STEP_SIZE,
			       OATH_TOTP_DEFAULT_START_TIME, 8, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate_time: %d\n", rc);
	  return 1;
	}

#if DEBUG
      printf ("otp[%ld]: %s\n", (unsigned long) tv[i].T, otp);
#endif

      if (strcmp (otp, tv[i].otp) != 0)
	{
	  printf ("otp[%d] got %s expected %s\n",
		  i, otp, tv[i].otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].otp, "65353130") == 0)
	    printf ("Mismatch due to 32-bit time_t...\n");
	  else
	    return 1;
	}

      rc = oath_totp_generate (secret, secretlen, tv[i].secs,
			       0, 0, 8, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate_time2: %d\n", rc);
	  return 1;
	}


      if (strcmp (otp, tv[i].otp) != 0)
	{
	  printf ("otp[%d] got %s expected2 %s\n",
		  i, otp, tv[i].otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].otp, "65353130") == 0)
	    printf ("Mismatch due to 32-bit time_t...\n");
	  else
	    return 1;
	}
    }

  rc = oath_done ();
  if (rc != OATH_OK)
    {
      printf ("oath_done: %d\n", rc);
      return 1;
    }

  return 0;
}
