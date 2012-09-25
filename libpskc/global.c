/*
 * global.c - implementation of library global functions
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
 *
 */

#include <config.h>

#include "pskc.h"

#include <string.h>		/* strverscmp */
#include <libxml/parser.h>	/* xmlInitParser */

/**
 * pskc_init:
 *
 * This function initializes the PSKC library.  Every user of this
 * library needs to call this function before using other functions.
 * You should call pskc_done() when use of the PSKC library is no
 * longer needed.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
pskc_init (void)
{
  xmlInitParser ();
  return PSKC_OK;
}

/**
 * pskc_done:
 *
 * This function deinitializes the PSKC library, which were
 * initialized using pskc_init().  After calling this function, no
 * other PSKC library function may be called except for to
 * re-initialize the library using pskc_init().
 *
 * Returns: On success, %PSKC_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
pskc_done (void)
{
  return PSKC_OK;
}

/**
 * pskc_check_version:
 * @req_version: version string to compare with, or NULL.
 *
 * Check PSKC library version.
 *
 * See %PSKC_VERSION for a suitable @req_version string.
 *
 * This function is one of few in the library that can be used without
 * a successful call to pskc_init().
 *
 * Return value: Check that the version of the library is at
 *   minimum the one given as a string in @req_version and return the
 *   actual version string of the library; return NULL if the
 *   condition is not met.  If NULL is passed to this function no
 *   check is done and only the version string is returned.
 **/
const char *
pskc_check_version (const char *req_version)
{
  if (!req_version || strverscmp (req_version, PSKC_VERSION) <= 0)
    return PSKC_VERSION;

  return NULL;
}
