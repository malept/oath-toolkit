/*
 * enums.c - support functions for PSKC enums.
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

#include <pskc/pskc.h>
#include "internal.h"

#include <string.h>		/* strcmp */

/**
 * pskc_pinusagemode2str:
 * @pinusagemode: an #pskc_pinusagemode enumeration type
 *
 * Convert #pskc_pinusagemode to a string.  For example,
 * pskc_pinusagemode2str(%PSKC_PINUSAGEMODE_LOCAL) will return
 * "Local".  The returned string must not be deallocated.
 *
 * Returns: String corresponding to #pskc_pinusagemode.
 */
const char *
pskc_pinusagemode2str (pskc_pinusagemode pinusagemode)
{
  const char *p;

  switch (pinusagemode)
    {
    case PSKC_PINUSAGEMODE_LOCAL:
      p = "Local";
      break;

    case PSKC_PINUSAGEMODE_PREPEND:
      p = "Prepend";
      break;

    case PSKC_PINUSAGEMODE_APPEND:
      p = "Append";
      break;

    case PSKC_PINUSAGEMODE_ALGORITHMIC:
      p = "Algorithmic";
      break;

    default:
      _pskc_debug ("unknown pinusagemode value %d\n", pinusagemode);
      p = "Unknown";
      break;
    }

  return p;
}

/**
 * pskc_valueformat2str:
 * @valueformat: an #pskc_valueformat enumeration type
 *
 * Convert #pskc_valueformat to a string.  For example,
 * pskc_valueformat2str(%PSKC_VALUEFORMAT_DECIMAL) will return
 * "DECIMAL".  The returned string must not be deallocated.
 *
 * Returns: String corresponding to #pskc_valueformat.
 */
const char *
pskc_valueformat2str (pskc_valueformat valueformat)
{
  const char *p;

  switch (valueformat)
    {
    case PSKC_VALUEFORMAT_DECIMAL:
      p = "DECIMAL";
      break;

    case PSKC_VALUEFORMAT_HEXADECIMAL:
      p = "HEXADECIMAL";
      break;

    case PSKC_VALUEFORMAT_ALPHANUMERIC:
      p = "ALPHANUMERIC";
      break;

    case PSKC_VALUEFORMAT_BASE64:
      p = "BASE64";
      break;

    case PSKC_VALUEFORMAT_BINARY:
      p = "BINARY";
      break;

    default:
      _pskc_debug ("unknown valueformat value %d\n", valueformat);
      p = "Unknown";
      break;
    }

  return p;
}

/**
 * pskc_keyusage2str:
 * @keyusage: an #pskc_keyusage enumeration type
 *
 * Convert #pskc_keyusage to a string.  For example,
 * pskc_keyusage2str(%PSKC_KEYUSAGE_OTP) will return "OTP".  The
 * returned string must not be deallocated.
 *
 * Returns: String corresponding to #pskc_keyusage.
 */
const char *
pskc_keyusage2str (pskc_keyusage keyusage)
{
  const char *p;

  switch (keyusage)
    {
    case PSKC_KEYUSAGE_OTP:
      p = "OTP";
      break;

    case PSKC_KEYUSAGE_CR:
      p = "CR";
      break;

    case PSKC_KEYUSAGE_ENCRYPT:
      p = "Encrypt";
      break;

    case PSKC_KEYUSAGE_INTEGRITY:
      p = "Integrity";
      break;

    case PSKC_KEYUSAGE_VERIFY:
      p = "Verify";
      break;

    case PSKC_KEYUSAGE_UNLOCK:
      p = "Unlock";
      break;

    case PSKC_KEYUSAGE_DECRYPT:
      p = "Decrypt";
      break;

    case PSKC_KEYUSAGE_KEYWRAP:
      p = "KeyWrap";
      break;

    case PSKC_KEYUSAGE_UNWRAP:
      p = "Unwrap";
      break;

    case PSKC_KEYUSAGE_DERIVE:
      p = "Derive";
      break;

    case PSKC_KEYUSAGE_GENERATE:
      p = "Generate";
      break;

    default:
      _pskc_debug ("unknown keyusage value %d\n", keyusage);
      p = "Unknown";
      break;
    }

  return p;
}

/**
 * pskc_str2pinusagemode:
 * @pinusagemode: an string describing a key usage.
 *
 * Convert a string to a #pskc_pinusagemode type.  For example,
 * pskc_str2pinusagemode("Local") will return
 * %PSKC_PINUSAGEMODE_LOCAL.
 *
 * Returns: The corresponding #pskc_pinusagemode value.
 */
pskc_pinusagemode
pskc_str2pinusagemode (const char *pinusagemode)
{
  if (strcmp ("Local", pinusagemode) == 0)
    return PSKC_PINUSAGEMODE_LOCAL;
  else if (strcmp ("Prepend", pinusagemode) == 0)
    return PSKC_PINUSAGEMODE_PREPEND;
  else if (strcmp ("Append", pinusagemode) == 0)
    return PSKC_PINUSAGEMODE_APPEND;
  else if (strcmp ("Algorithmic", pinusagemode) == 0)
    return PSKC_PINUSAGEMODE_ALGORITHMIC;
  _pskc_debug ("unknown pinusagemode value '%s'\n", pinusagemode);
  return PSKC_PINUSAGEMODE_UNKNOWN;
}

/**
 * pskc_str2valueformat:
 * @valueformat: an string describing a key usage.
 *
 * Convert a string to a #pskc_valueformat type.  For example,
 * pskc_str2valueformat("DECIMAL") will return
 * %PSKC_VALUEFORMAT_DECIMAL.
 *
 * Returns: The corresponding #pskc_valueformat value.
 */
pskc_valueformat
pskc_str2valueformat (const char *valueformat)
{
  if (strcmp ("DECIMAL", valueformat) == 0)
    return PSKC_VALUEFORMAT_DECIMAL;
  else if (strcmp ("HEXADECIMAL", valueformat) == 0)
    return PSKC_VALUEFORMAT_HEXADECIMAL;
  else if (strcmp ("ALPHANUMERIC", valueformat) == 0)
    return PSKC_VALUEFORMAT_ALPHANUMERIC;
  else if (strcmp ("BASE64", valueformat) == 0)
    return PSKC_VALUEFORMAT_BASE64;
  else if (strcmp ("BINARY", valueformat) == 0)
    return PSKC_VALUEFORMAT_BINARY;
  _pskc_debug ("unknown valueformat value '%s'\n", valueformat);
  return PSKC_VALUEFORMAT_UNKNOWN;
}

/**
 * pskc_str2keyusage:
 * @keyusage: an string describing a key usage.
 *
 * Convert a string to a #pskc_keyusage type.  For example,
 * pskc_str2keyusage("KeyWrap") will return %PSKC_KEYUSAGE_KEYWRAP.
 *
 * Returns: The corresponding #pskc_keyusage value.
 */
pskc_keyusage
pskc_str2keyusage (const char *keyusage)
{
  if (strcmp ("OTP", keyusage) == 0)
    return PSKC_KEYUSAGE_OTP;
  else if (strcmp ("CR", keyusage) == 0)
    return PSKC_KEYUSAGE_CR;
  else if (strcmp ("Encrypt", keyusage) == 0)
    return PSKC_KEYUSAGE_ENCRYPT;
  else if (strcmp ("Integrity", keyusage) == 0)
    return PSKC_KEYUSAGE_INTEGRITY;
  else if (strcmp ("Verify", keyusage) == 0)
    return PSKC_KEYUSAGE_VERIFY;
  else if (strcmp ("Unlock", keyusage) == 0)
    return PSKC_KEYUSAGE_UNLOCK;
  else if (strcmp ("Decrypt", keyusage) == 0)
    return PSKC_KEYUSAGE_DECRYPT;
  else if (strcmp ("KeyWrap", keyusage) == 0)
    return PSKC_KEYUSAGE_KEYWRAP;
  else if (strcmp ("Unwrap", keyusage) == 0)
    return PSKC_KEYUSAGE_UNWRAP;
  else if (strcmp ("Derive", keyusage) == 0)
    return PSKC_KEYUSAGE_DERIVE;
  else if (strcmp ("Generate", keyusage) == 0)
    return PSKC_KEYUSAGE_GENERATE;
  _pskc_debug ("unknown keyusage value '%s'\n", keyusage);
  return PSKC_KEYUSAGE_UNKNOWN;
}
