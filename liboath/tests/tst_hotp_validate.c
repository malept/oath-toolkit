/*
 * tst_hotp_validate.c - self-tests for liboath HOTP validate functions
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

static int
my_strcmp (void *handle, const char *test_otp)
{
  const char *otp = handle;
  return strcmp (otp, test_otp) == 0 ? 0 : 1;
}

int
main (void)
{
  int rc;
  char secret[20] = "\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30"
    "\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30";
  size_t secretlen = sizeof (secret);
  uint64_t moving_factor;
  unsigned digits;

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  for (digits = 6; digits <= 8; digits++)
    for (moving_factor = 0; moving_factor < MAX_ITER; moving_factor++)
      {
	size_t i;

	rc = oath_hotp_validate (secret, secretlen, 0, 20, expect[digits][moving_factor]);
	if (rc != moving_factor)
	  {
	    printf ("validate failed on digits %d moving factor %ld\n",
		    digits, (long) moving_factor);
	    return 1;
	  }

	for (i = 0; i < moving_factor; i++)
	  {
	    rc = oath_hotp_validate (secret, secretlen, 0, i, expect[digits][moving_factor]);
	    if (rc != OATH_INVALID_OTP)
	      {
		printf ("unexpected return code %d for digits %d and iter %d\n",
			rc, digits, i);
		return 1;
	      }
	  }

	rc = oath_hotp_validate_callback (secret, secretlen, 0, 20,
					  strlen (expect[digits][moving_factor]),
					  my_strcmp,
					  (void *) expect[digits][moving_factor]);
	if (rc != moving_factor)
	  {
	    printf ("validate failed on digits %d moving factor %ld\n",
		    digits, (long) moving_factor);
	    return 1;
	  }

	for (i = 0; i < moving_factor; i++)
	  {
	    rc = oath_hotp_validate_callback (secret, secretlen, 0, i,
					      strlen (expect[digits][moving_factor]),
					      my_strcmp,
					      (void *) expect[digits][moving_factor]);
	    if (rc != OATH_INVALID_OTP)
	      {
		printf ("unexpected return code %d for digits %d and iter %d\n",
			rc, digits, i);
		return 1;
	      }
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
