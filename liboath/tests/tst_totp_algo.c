/*
 * tst_totp_algo.c - self-tests for liboath TOTP algorithm functions
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
#define DEBUG 1

/* *INDENT-OFF* */
const struct {
  time_t secs;
  uint64_t T;
  char *otp;
  char *sha256otp;
  char *sha512otp;
} tv[] = {
  /* From RFC 6238. */
  { 59, 0x0000000000000001, "94287082", "46119246", "90693936" },
  { 1111111109, 0x00000000023523EC, "07081804", "68084774", "25091201" },
  { 1111111111, 0x00000000023523ED, "14050471", "67062674", "99943326" },
  { 1234567890, 0x000000000273EF07, "89005924", "91819424", "93441116" },
  { 2000000000, 0x0000000003F940AA, "69279037", "90698825", "38618901" },
  { 20000000000, 0x0000000027BC86AA, "65353130", "77737706", "47863826" }
};
/* *INDENT-ON* */

int
main (void)
{
  oath_rc rc;
  char secret[64] = "12345678901234567890123456789"
    "01234567890123456789012345678901234";
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
      rc = oath_totp_generate (secret, 20, tv[i].secs,
			       OATH_TOTP_DEFAULT_TIME_STEP_SIZE,
			       OATH_TOTP_DEFAULT_START_TIME, 8, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate: %d\n", rc);
	  return 1;
	}

#if DEBUG
      printf ("otp[%ld]: %s\n", (unsigned long) tv[i].T, otp);
#endif

      if (strcmp (otp, tv[i].otp) != 0)
	{
	  printf ("otp[%ld] got %s expected %s\n", i, otp, tv[i].otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].otp, "65353130") == 0)
	    printf ("Mismatch due to 32-bit time_t...\n");
	  else
	    return 1;
	}

      rc = oath_totp_generate (secret, 20, tv[i].secs, 0, 0, 8, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate 2: %d\n", rc);
	  return 1;
	}

      if (strcmp (otp, tv[i].otp) != 0)
	{
	  printf ("otp[%ld] got %s expected2 %s\n", i, otp, tv[i].otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].otp, "65353130") == 0)
	    printf ("Mismatch due to 32-bit time_t...\n");
	  else
	    return 1;
	}

      rc = oath_totp_generate2 (secret, 32, tv[i].secs, 0, 0, 8,
				OATH_TOTP_HMAC_SHA256, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate2 sha256: %d\n", rc);
	  return 1;
	}

#if DEBUG
      printf ("otp[%ld]: %s\n", (unsigned long) tv[i].T, otp);
#endif

      if (strcmp (otp, tv[i].sha256otp) != 0)
	{
	  printf ("otp[%ld] got %s expected2 %s\n", i, otp, tv[i].sha256otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].sha256otp, "65353130") == 0)
	    printf ("Mismatch due to 32-bit time_t...\n");
	  else
	    return 1;
	}

      rc = oath_totp_generate2 (secret, 64, tv[i].secs, 0, 0, 8,
				OATH_TOTP_HMAC_SHA512, otp);
      if (rc != OATH_OK)
	{
	  printf ("oath_totp_generate2 sha512: %d\n", rc);
	  return 1;
	}

#if DEBUG
      printf ("otp[%ld]: %s\n", (unsigned long) tv[i].T, otp);
#endif

      if (strcmp (otp, tv[i].sha512otp) != 0)
	{
	  printf ("otp[%ld] got %s expected2 %s\n", i, otp, tv[i].sha512otp);
	  if (strcmp (otp, "82762030") == 0
	      && strcmp (tv[i].sha512otp, "65353130") == 0)
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
