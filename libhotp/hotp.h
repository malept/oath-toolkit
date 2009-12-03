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

/**
 * HOTP_VERSION
 *
 * Pre-processor symbol with a string that describe the header file
 * version number.  Used together with hotp_check_version() to verify
 * header file and run-time library consistency.
 */
# define HOTP_VERSION "1.0.0"

/**
 * HOTP_VERSION_MAJOR
 *
 * Pre-processor symbol with a decimal value that describe the major
 * level of the header file version number.  For example, when the
 * header version is 1.2.3 this symbol will be 1.
 */
# define HOTP_VERSION_MAJOR 0

/**
 * HOTP_VERSION_MINOR
 *
 * Pre-processor symbol with a decimal value that describe the minor
 * level of the header file version number.  For example, when the
 * header version is 1.2.3 this symbol will be 2.
 */
# define HOTP_VERSION_MINOR 0

/**
 * HOTP_VERSION_PATCH
 *
 * Pre-processor symbol with a decimal value that describe the patch
 * level of the header file version number.  For example, when the
 * header version is 1.2.3 this symbol will be 3.
 */
# define HOTP_VERSION_PATCH 0

/**
 * HOTP_VERSION_NUMBER
 *
 * Pre-processor symbol with a hexadecimal value describing the
 * header file version number.  For example, when the header version
 * is 1.2.3 this symbol will have the value 0x010203.
 */
# define HOTP_VERSION_NUMBER 0x010000

typedef enum {
  HOTP_OK = 0,
  HOTP_CRYPTO_ERROR,     /* 1 */
  HOTP_INVALID_DIGITS,   /* 2 */
  HOTP_PRINTF_ERROR,     /* 3 */
  HOTP_INVALID_HEX,      /* 4 */
  HOTP_TOO_SMALL_BUFFER  /* 5 */
} hotp_rc;

#define HOTP_DYNAMIC_TRUNCATION SIZE_MAX

#define HOTP_OTP_LENGTH(digits, checksum) (digits + (checksum ? 1 : 0))

extern HOTPAPI int hotp_init (void);
extern HOTPAPI int hotp_done (void);

extern HOTPAPI int hotp_generate_otp (const char *secret,
				      size_t secret_length,
				      uint64_t moving_factor,
				      unsigned digits,
				      bool add_checksum,
				      size_t truncation_offset,
				      char *output_otp);

extern HOTPAPI int hotp_validate_otp (const char *secret,
				      size_t secret_length,
				      uint64_t start_moving_factor,
				      size_t window,
				      unsigned digits,
				      const char *otp);

extern HOTPAPI int hotp_hex2bin (char *hexstr,
				 char *binstr,
				 size_t *binlen);

extern HOTPAPI const char *hotp_check_version (const char *req_version);

#endif /* HOTP_H */
