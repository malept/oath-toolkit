/*
 * tst_hotp.c - self-tests for libhotp
 * Copyright (C) 2009  Simon Josefsson
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

#include "hotp.h"

#include <stdio.h>

#define MAX_DIGIT 8
#define MAX_ITER 20

const char *expect[MAX_DIGIT + 1][MAX_ITER] = {
  /* digit 0 */
  {},
  /* digit 1 */
  {},
  /* digit 2 */
  {},
  /* digit 3 */
  {},
  /* digit 4 */
  {},
  /* digit 5 */
  {},
  /* digit 6 */
  {
    /* The first ten of these match the values in RFC 4226. */
    "755224",
    "287082",
    "359152",
    "969429",
    "338314",
    "254676",
    "287922",
    "162583",
    "399871",
    "520489",
    "403154",
    "481090",
    "868912",
    "736127",
    "229903",
    "436521",
    "186581",
    "447589",
    "903435",
    "578337"
  },
  /* digit 7 */
  {
    "4755224",
    "4287082",
    "7359152",
    "6969429",
    "0338314",
    "8254676",
    "8287922",
    "2162583",
    "3399871",
    "5520489",
    "2403154",
    "3481090",
    "7868912",
    "3736127",
    "5229903",
    "3436521",
    "2186581",
    "4447589",
    "1903435",
    "1578337",
  },
  /* digit 8 */
  {
    "84755224",
    "94287082",
    "37359152",
    "26969429",
    "40338314",
    "68254676",
    "18287922",
    "82162583",
    "73399871",
    "45520489",
    "72403154",
    "43481090",
    "47868912",
    "33736127",
    "35229903",
    "23436521",
    "22186581",
    "94447589",
    "71903435",
    "21578337",
  }
};

int
main (void)
{
  hotp_rc rc;
  char *secret = "12345678901234567890";
  char otp[10];
  uint64_t moving_factor;
  unsigned digits;

  rc = hotp_init ();
  if (rc != HOTP_OK)
    {
      printf ("hotp_init: %d\n", rc);
      return 1;
    }

  for (digits = 6; digits <= 8; digits++)
    for (moving_factor = 0; moving_factor < MAX_ITER; moving_factor++)
      {
	rc = hotp_generate_otp (secret, strlen (secret), moving_factor,
				digits, false, HOTP_DYNAMIC_TRUNCATION, otp);
	if (rc != HOTP_OK)
	  {
	    printf ("hotp_generate_otp: %d\n", rc);
	    return 1;
	  }

#if DEBUG
	printf ("otp[%d]: %s\n", (unsigned) moving_factor, otp);
#endif

	if (!expect[digits][moving_factor])
	  {
	    printf ("no test vector for digits %d counter %ld\n",
		    digits, (long) moving_factor);
	    return 1;
	  }

	if (strcmp (otp, expect[digits][moving_factor]) != 0)
	  {
	    printf ("otp[%d][%ld] got %s expected %s\n",
		    digits, (long) moving_factor, otp,
		    expect[digits][moving_factor]);
	    return 1;
	  }
      }

  rc = hotp_done ();
  if (rc != HOTP_OK)
    {
      printf ("hotp_done: %d\n", rc);
      return 1;
    }

  return 0;
}
