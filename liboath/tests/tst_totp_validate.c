/*
 * tst_totp_validate.c - self-tests for liboath TOTP validate functions
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

/* *INDENT-OFF* */
const struct {
  time_t now;
  unsigned window;
  char *otp;
  int expected_rc;
  int otp_pos;
  uint64_t otp_counter;
} tv[] = {
  /* Derived from RFC 6238. */
  { 0, 10, "94287082", 1, 1, 1},
  { 1111111100, 10, "07081804", 0, 0, 37037036},
  { 1111111109, 10, "07081804", 0, 0, 37037036},
  { 1111111000, 10, "07081804", 3, 3, 37037036},
  { 1111112000, 99, "07081804", 30, -30, 37037036},
  { 1111111100, 10, "14050471", 1, 1, 37037037},
  { 1111111109, 10, "14050471", 1, 1, 37037037},
  { 1111111000, 10, "14050471", 4, 4, 37037037},
  { 1111112000, 99, "14050471", 29, -29, 37037037},
};
/* *INDENT-ON* */

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
  unsigned time_step_size = 30;
  time_t start_offset = 0;
  unsigned digits;
  time_t now;
  size_t i;

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  for (i = 0; i < sizeof (tv) / sizeof (tv[0]); i++)
    {
      int otp_pos;
      uint64_t otp_counter;

      rc = oath_totp_validate (secret, secretlen, tv[i].now, time_step_size,
			       start_offset, tv[i].window, tv[i].otp);
      if (rc != tv[i].expected_rc)
	{
	  printf ("validate loop %ld failed (rc %d != %d)?!\n",
		  i, rc, tv[i].expected_rc);
	  return 1;
	}

      rc = oath_totp_validate_callback (secret, secretlen, tv[i].now,
					time_step_size,
					start_offset, 8, tv[i].window,
					my_strcmp, (void *) tv[i].otp);
      if (rc != tv[i].expected_rc)
	{
	  printf ("validate_callback loop %ld failed (rc %d != %d)?!\n",
		  i, rc, tv[i].expected_rc);
	  return 1;
	}

      otp_pos = 23;

      rc = oath_totp_validate2 (secret, secretlen, tv[i].now, time_step_size,
				start_offset, tv[i].window, &otp_pos,
				tv[i].otp);
      if (rc != tv[i].expected_rc)
	{
	  printf ("validate2 loop %ld failed (rc %d != %d)?!\n",
		  i, rc, tv[i].expected_rc);
	  return 1;
	}
      if (otp_pos != tv[i].otp_pos)
	{
	  printf ("validate2 loop %ld failed (pos %d != %d)?!\n",
		  i, otp_pos, tv[i].otp_pos);
	  return 1;
	}

      otp_pos = 47;

      rc = oath_totp_validate2_callback (secret, secretlen, tv[i].now,
					 time_step_size, start_offset,
					 8, tv[i].window, &otp_pos,
					 my_strcmp, (void *) tv[i].otp);
      if (rc != tv[i].expected_rc)
	{
	  printf ("validate2_callback loop %ld failed (rc %d != %d)?!\n",
		  i, rc, tv[i].expected_rc);
	  return 1;
	}
      if (otp_pos != tv[i].otp_pos)
	{
	  printf ("validate2_callback loop %ld failed (pos %d != %d)?!\n",
		  i, otp_pos, tv[i].otp_pos);
	  return 1;
	}
	
      otp_pos = 95;
      otp_counter = 23;

      rc = oath_totp_validate3 (secret, secretlen, tv[i].now, time_step_size,
				start_offset, tv[i].window, &otp_pos, &otp_counter,
				tv[i].otp);
      if (rc != tv[i].expected_rc)
	{
	  printf ("validate3 loop %ld failed (rc %d != %d)?!\n",
		  i, rc, tv[i].expected_rc);
	  return 1;
	}
      if (otp_pos != tv[i].otp_pos)
	{
	  printf ("validate3 loop %ld failed (pos %d != %d)?!\n",
		  i, otp_pos, tv[i].otp_pos);
	  return 1;
	}
	  if (otp_counter != tv[i].otp_counter)
	{
		printf ("validate3 loop %ld failed (counter %d != %d)?!\n",
			i, otp_counter, tv[i].otp_counter);
		return 1;
	}

	  otp_pos = 191;
	  otp_counter=47;

	  rc = oath_totp_validate3_callback (secret, secretlen, tv[i].now,
			  		 time_step_size, start_offset,
					 8, tv[i].window, &otp_pos,
					 &otp_counter, my_strcmp, (void *) tv[i].otp);

	  if (rc != tv[i].expected_rc)
	{
		printf ("validate3_callback loop %ld failed (rc %d != %d)?!\n",
			i, rc, tv[i].expected_rc);
		return 1;
	}
	  if (otp_pos != tv[i].otp_pos)
	{
		printf ("validate3_callback loop %ld failed (pos %d != %d)?!\n",
			i, otp_pos, tv[i].otp_pos);
		return 1;
	}
	  if (otp_counter != tv[i].otp_counter)
	{
		printf ("validate3_callback loop %ld failed (counter %d != %d)?!\n",
		i, otp_counter, tv[i].otp_counter);
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
