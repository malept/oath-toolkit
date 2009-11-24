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

#ifndef HOTP_H
# define HOTP_H

# ifndef HOTPAPI
#  if defined LIBHOTP_BUILDING && defined HAVE_VISIBILITY && HAVE_VISIBILITY
#   define HOTPAPI __attribute__((__visibility__("default")))
#  elif defined LIBHOTP_BUILDING && defined _MSC_VER && ! defined LIBHOTP_STATIC
#   define HOTPAPI __declspec(dllexport)
#  elif defined _MSC_VER && ! defined LIBHOTP_STATIC
#   define HOTPAPI __declspec(dllimport)
#  else
#   define HOTPAPI
#  endif
# endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef enum {
  HOTP_OK = 0,
  HOTP_CRYPTO_ERROR,
  HOTP_INVALID_DIGITS,
  HOTP_PRINTF_ERROR
} hotp_rc;

#define HOTP_DYNAMIC_TRUNCATION SIZE_MAX

extern int HOTPAPI hotp_init (void);
extern int HOTPAPI hotp_done (void);

#define HOTP_OTP_LENGTH(digits, checksum) (digits + (checksum ? 1 : 0))

extern int HOTPAPI hotp_generate_otp (char *secret,
				      size_t secret_length,
				      uint64_t moving_factor,
				      unsigned digits,
				      bool add_checksum,
				      size_t truncation_offset,
				      char *output_otp);

#endif /* HOTP_H */
