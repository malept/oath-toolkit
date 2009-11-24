/*
 * hotp.c - implementation of the HOTP algorithm
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

#include "gc.h"
#include <stdio.h>

/**
 * hotp_init - initialize the HOTP library
 *
 * This function initializes the HOTP library.  Every user of this
 * library needs to call this function before using other functions.
 * You should call hotp_done() when use of the HOTP library is no
 * longer needed.
 *
 * Note that this function may also initialize Libgcrypt, if the HOTP
 * library is built with libgcrypt support and libgcrypt has not been
 * initialized before.  Thus if you want to manually initialize
 * libgcrypt you must do it before calling this function.  This is
 * useful in cases you want to disable libgcrypt's internal lockings
 * etc.
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_init (void)
{
  if (gc_init () != GC_OK)
    return HOTP_CRYPTO_ERROR;

  return HOTP_OK;
}

/**
 * hotp_done - deinitialize the HOTP library
 *
 * This function deinitializes the HOTP library, which were
 * initialized using hotp_init().  After calling this function, no
 * other HOTP library function may be called except for to
 * re-initialize the library using hotp_init().
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_done (void)
{
  gc_done ();

  return HOTP_OK;
}

/**
 * hotp_generate_otp:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @moving_factor: a counter indicating the current OTP to generate
 * @digits: number of requested digits in the OTP, excluding checksum
 * @add_checksum: whether to add a checksum digit or not
 * @truncation_offset: use a specific truncation offset
 * @output_otp: output buffer, must have room for the output OTP plus zero
 *
 * Generate a one-time-password using the HOTP algorithm as described
 * in RFC 4226.
 *
 * Use a value of %HOTP_DYNAMIC_TRUNCATION for @truncation_offset
 * unless you really need a specific truncation offset.
 *
 * To find out the size of the OTP you may use the HOTP_OTP_LENGTH()
 * macro.  The @output_otp buffer must be have room for that length
 * plus one for the terminating NUL.
 *
 * Currently only values 6, 7 and 8 for @digits are supported, and the
 * @add_checksum value is ignored.  These restrictions may be lifted
 * in future versions, although some limitations are inherent in the
 * protocol.
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_generate_otp (char *secret,
		   size_t secret_length,
		   uint64_t moving_factor,
		   unsigned digits,
		   bool add_checksum,
		   size_t truncation_offset,
		   char *output_otp)
{
  char hs[GC_SHA1_DIGEST_SIZE];
  long S;

  {
    char counter[sizeof (moving_factor)];
    size_t i;
    Gc_rc rc;

    for (i = 0; i < sizeof (counter); i++)
      counter[i] =
	(moving_factor >> ((sizeof (moving_factor) - i - 1) * 8)) & 0xFF;

    rc = gc_hmac_sha1 (secret, secret_length,
		       counter, sizeof (moving_factor), hs);
    if (rc != GC_OK)
      return HOTP_CRYPTO_ERROR;
  }

  {
    uint8_t offset = hs[sizeof (hs) - 1] & 0x0f;

    S = (((hs[offset] & 0x7f) << 24)
	 | ((hs[offset + 1] & 0xff) << 16)
	 | ((hs[offset + 2] & 0xff) << 8)
	 | ((hs[offset + 3] & 0xff)));

#if DEBUG
    printf ("offset is %d hash is ", offset);
    for (i = 0; i < 20; i++)
      printf ("%02x ", hs[i] & 0xFF);
    printf ("\n");

    printf ("value: %d\n", S);
#endif
  }

  switch (digits)
    {
    case 6:
      S = S % 1000000;
      break;

    case 7:
      S = S % 10000000;
      break;

    case 8:
      S = S % 100000000;
      break;

    default:
      return HOTP_INVALID_DIGITS;
      break;
    }

  {
    int len = snprintf (output_otp, digits + 1, "%.*ld", digits, S);
    output_otp[digits] = '\0';
    if (len != digits)
      return HOTP_PRINTF_ERROR;
  }

  return HOTP_OK;
}
