/* error.c - liboath error handling helpers.
 * Copyright (C) 2011 Simon Josefsson
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
 */

#include <config.h>

#include "oath.h"

/* Prepare for gettext. */
#define _(x) x
#define N_(x) x
#define bindtextdomain(a,b) 0

#define ERR(name, desc) { name, #name, desc }

typedef struct
{
  int rc;
  const char *name;
  const char *description;
} err_t;

static const err_t errors[] = {
  ERR (OATH_OK, N_("Successful return")),
  ERR (OATH_CRYPTO_ERROR, N_("Internal error in crypto functions")),
  ERR (OATH_INVALID_DIGITS, N_("Unsupported number of OTP digits")),
  ERR (OATH_PRINTF_ERROR, N_("Error from system printf call")),
  ERR (OATH_INVALID_HEX, N_("Hex string is invalid")),
  ERR (OATH_TOO_SMALL_BUFFER, N_("The output buffer is too small")),
  ERR (OATH_INVALID_OTP, N_("The OTP is not valid")),
  ERR (OATH_REPLAYED_OTP, N_("The OTP has been replayed")),
  ERR (OATH_BAD_PASSWORD, N_("The password does not match")),
  ERR (OATH_INVALID_COUNTER, N_("The counter value is corrupt")),
  ERR (OATH_INVALID_TIMESTAMP, N_("The timestamp is corrupt")),
  ERR (OATH_NO_SUCH_FILE, N_("The supplied filename does not exist")),
  ERR (OATH_UNKNOWN_USER, N_("Cannot find information about user")),
  ERR (OATH_FILE_SEEK_ERROR, N_("System error when seeking in file")),
  ERR (OATH_FILE_CREATE_ERROR, N_("System error when creating file")),
  ERR (OATH_FILE_LOCK_ERROR, N_("System error when locking file")),
  ERR (OATH_FILE_RENAME_ERROR, N_("System error when renaming file")),
  ERR (OATH_FILE_UNLINK_ERROR, N_("System error when removing file")),
  ERR (OATH_TIME_ERROR, N_("System error for time manipulation"))
};

/**
 * oath_strerror:
 * @err: liboath error code
 *
 * Convert return code to human readable string explanation of the
 * reason for the particular error code.
 *
 * This string can be used to output a diagnostic message to the user.
 *
 * This function is one of few in the library that can be used without
 * a successful call to oath_init().
 *
 * Return value: Returns a pointer to a statically allocated string
 *   containing an explanation of the error code @err.
 *
 * Since: 1.8.0
 **/
const char *
oath_strerror (int err)
{
  static const char *unknown = N_("Liboath unknown error");
  const char *p;

  bindtextdomain (PACKAGE, LOCALEDIR);

  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    return _(unknown);

  p = errors[-err].description;
  if (!p)
    p = unknown;

  return _(p);
}


/**
 * oath_strerror_name:
 * @err: liboath error code
 *
 * Convert return code to human readable string representing the error
 * code symbol itself.  For example, oath_strerror_name(%OATH_OK)
 * returns the string "OATH_OK".
 *
 * This string can be used to output a diagnostic message to the user.
 *
 * This function is one of few in the library that can be used without
 * a successful call to oath_init().
 *
 * Return value: Returns a pointer to a statically allocated string
 *   containing a string version of the error code @err, or %NULL if
 *   the error code is not known.
 *
 * Since: 1.8.0
 **/
const char *
oath_strerror_name (int err)
{
  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    return NULL;

  return errors[-err].name;
}
