/*
 * pskc/errors.h - PSKC header file with error handling prototypes.
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

#ifndef PSKC_ERRORS_H
#define PSKC_ERRORS_H

/**
 * SECTION:errors
 * @short_description: Error handling.
 *
 * Most library functions uses an int return value to indicate success
 * or failure, using #pskc_rc values.  The values can be converted
 * into human readable explanations using pskc_strerror().  The
 * symbolic error codes can be converted into strings using
 * pskc_strerror_name().
 */

/**
 * pskc_rc:
 * @PSKC_OK: Successful return.
 * @PSKC_MALLOC_ERROR: Memory allocation failed.
 * @PSKC_XML_ERROR: Error returned from XML library.
 * @PSKC_PARSE_ERROR: Error parsing PSKC data.
 * @PSKC_BASE64_ERROR: Error decoding base64 data.
 * @PSKC_UNKNOWN_OUTPUT_FORMAT: Unknown output format.
 * @PSKC_LAST_ERROR: Meta-error indicating the last error code, for use
 *   when iterating over all error codes or similar.
 *
 * Return codes for PSKC functions.  All return codes are negative
 * except for the successful code %PSKC_OK which are guaranteed to be
 * 0.  Positive values are reserved for non-error return codes.
 *
 * Note that the #pskc_rc enumeration may be extended at a later date
 * to include new return codes.
 */
typedef enum
{
  PSKC_OK = 0,
  PSKC_MALLOC_ERROR = -1,
  PSKC_XML_ERROR = -2,
  PSKC_PARSE_ERROR = -3,
  PSKC_BASE64_ERROR = -4,
  PSKC_UNKNOWN_OUTPUT_FORMAT = -5,
  /* When adding anything above, you need to update errors.c and
     the following constant. */
  PSKC_LAST_ERROR = -5
} pskc_rc;

extern PSKCAPI const char *pskc_strerror (int err);
extern PSKCAPI const char *pskc_strerror_name (int err);

#endif /* PSKC_ERRORS_H */
