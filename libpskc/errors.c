/* error.c - libpskc error handling helpers.
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

#include <stddef.h>		/* NULL */

#include "pskc.h"

#define ERR(name, desc) { name, #name, desc }

typedef struct
{
  int rc;
  const char *name;
  const char *description;
} err_t;

static const err_t errors[] = {
  ERR (PSKC_OK, "Successful return"),
  ERR (PSKC_MALLOC_ERROR, "Memory allocation failed"),
  ERR (PSKC_XML_PARSE_ERROR, "XML parse error"),
  ERR (PSKC_XML_SYNTAX_ERROR, "Syntax error in XML data"),
  ERR (PSKC_UNKNOWN_OUTPUT_FORMAT, "Unknown output format")
};

/**
 * pskc_strerror:
 * @err: libpskc error code
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
 *
 * Since: 2.0.0
 **/
const char *
pskc_strerror (int err)
{
  static const char *unknown = "Libpskc unknown error";
  const char *p;

  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    return unknown;

  p = errors[-err].description;
  if (!p)
    p = unknown;

  return p;
}


/**
 * pskc_strerror_name:
 * @err: libpskc error code
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
 *   containing a string version of the error code @err, or %NULL if
 *   the error code is not known.
 *
 * Since: 2.0.0
 **/
const char *
pskc_strerror_name (int err)
{
  if (-err < 0 || -err >= (int) (sizeof (errors) / sizeof (errors[0])))
    return NULL;

  return errors[-err].name;
}
