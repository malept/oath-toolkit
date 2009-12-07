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

int
main (void)
{
  hotp_rc rc;
  time_t last_otp;

  if (!hotp_check_version (HOTP_VERSION))
    {
      printf ("hotp_check_version (%s) failed [%s]\n", HOTP_VERSION,
	      hotp_check_version (NULL));
      return 1;
    }

  rc = hotp_init ();
  if (rc != HOTP_OK)
    {
      printf ("hotp_init: %d\n", rc);
      return 1;
    }

  rc = hotp_authenticate_otp_usersfile ("users.hotp",
					"joe",
					"755224",
					0,
					"1234",
					&last_otp);
  if (rc != HOTP_BAD_PASSWORD)
    {
      printf ("hotp_authenticate_otp_usersfile: %d\n", rc);
      return 1;
    }

  rc = hotp_authenticate_otp_usersfile ("users.hotp",
					"foo",
					"755224",
					0,
					"8989",
					&last_otp);
  if (rc != HOTP_REPLAYED_OTP)
    {
      printf ("hotp_authenticate_otp_usersfile: %d\n", rc);
      return 1;
    }
  if (last_otp != 1260203142)
    {
      printf ("hotp_authenticate_otp_usersfile timestamp %d != 1260203142\n",
	      last_otp);
      return 1;
    }

  rc = hotp_authenticate_otp_usersfile ("users.hotp",
					"rms",
					"755224",
					0,
					"4321",
					&last_otp);
  if (rc != HOTP_BAD_PASSWORD)
    {
      printf ("hotp_authenticate_otp_usersfile: %d\n", rc);
      return 1;
    }

  rc = hotp_authenticate_otp_usersfile ("users.hotp",
					"rms",
					"436521",
					10,
					"6767",
					&last_otp);
  if (rc != HOTP_OK)
    {
      printf ("hotp_authenticate_otp_usersfile: %d\n", rc);
      return 1;
    }

  rc = hotp_done ();
  if (rc != HOTP_OK)
    {
      printf ("hotp_done: %d\n", rc);
      return 1;
    }

  return 0;
}
