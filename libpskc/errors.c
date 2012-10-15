/*
 * error.c - libpskc error handling helpers.
 * Copyright (C) 2012 Simon Josefsson
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

#include <pskc/pskc.h>
#include "internal.h"

#define ERR(name, desc) { name, #name, desc }

typedef struct
{
  int rc;
  const char *name;
  const char *description;
} err_t;

static const err_t errors[] = {
  ERR (PSKC_OK, "success"),
  ERR (PSKC_MALLOC_ERROR, "memory allocation failed"),
  ERR (PSKC_XML_ERROR, "error returned from XML library"),
  ERR (PSKC_PARSE_ERROR, "error parsing PSKC data"),
  ERR (PSKC_BASE64_ERROR, "error base64 decoding data"),
  ERR (PSKC_UNKNOWN_OUTPUT_FORMAT, "unknown output format"),
  ERR (PSKC_XMLSEC_ERROR, "error returned from XMLSec library")
};

/**
 * pskc_strerror:
 * @err: error code, a #pskc_rc value.
 *
 * Convert return code to human readable string explanation of the
 * reason for the particular error code.
 *
 * This string can be used to output a diagnostic message to the user.
 *
 * This function is one of few in the library that can be used without
 * a successful call to pskc_init().
 *
 * Return value: Returns a pointer to a statically allocated string
 *   containing an explanation of the error code @err.
 **/
const char *
pskc_strerror (int err)
{
  static const char *unknown = "Libpskc unknown error";
  const char *p;

  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    {
      _pskc_debug ("out of range error code %d\n", err);
      return unknown;
    }

  p = errors[-err].description;
  if (!p)
    {
      _pskc_debug ("missing description field for error code %d\n", err);
      p = unknown;
    }

  return p;
}


/**
 * pskc_strerror_name:
 * @err: error code, a #pskc_rc value.
 *
 * Convert return code to human readable string representing the error
 * code symbol itself.  For example, pskc_strerror_name(%PSKC_OK)
 * returns the string "PSKC_OK".
 *
 * This string can be used to output a diagnostic message to the user.
 *
 * This function is one of few in the library that can be used without
 * a successful call to pskc_init().
 *
 * Return value: Returns a pointer to a statically allocated string
 *   containing a string version of the error code @err, or NULL if
 *   the error code is not known.
 **/
const char *
pskc_strerror_name (int err)
{
  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    {
      _pskc_debug ("attempted naming out of range error code %d\n", err);
      return NULL;
    }

  return errors[-err].name;
}
