/*
 * pskc/enums.h - PSKC header file with enum prototypes.
 * Copyright (C) 2012-2013 Simon Josefsson
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

#ifndef PSKC_ENUMS_H
#define PSKC_ENUMS_H

/**
 * SECTION:enums
 * @short_description: PSKC value enumerations and related functions.
 *
 * The #pskc_pinusagemode type describes PIN Policy Usage Modes.  You
 * can convert between string representation and integer values using
 * pskc_pinusagemode2str() and pskc_str2pinusagemode().
 *
 * The #pskc_valueformat type describes PSKC data value encodings.
 * You can convert between string representation and integer values
 * using pskc_valueformat2str() and pskc_str2valueformat().
 *
 * The #pskc_keyusage type describes what PSKC keys may be used for.
 * You can convert between string representation and integer values
 * using pskc_keyusage2str() and pskc_str2keyusage().  Note that often
 * multiple #pskc_keyusage values are ORed together to form set of
 * values.
 */

/**
 * pskc_pinusagemode:
 * @PSKC_PINUSAGEMODE_UNKNOWN: Unknown mode.
 * @PSKC_PINUSAGEMODE_LOCAL: PIN is checked locally on the device.
 * @PSKC_PINUSAGEMODE_PREPEND: PIN is prepended to the OTP and checked
 *   by OTP validating party.
 * @PSKC_PINUSAGEMODE_APPEND: PIN is appended to the OTP and checked
 *   by OTP validating party.
 * @PSKC_PINUSAGEMODE_ALGORITHMIC: The PIN is used as part of the
 * algorithm computation.
 * @PSKC_PINUSAGEMODE_LAST: Meta-value corresponding to the highest value,
 * for use in iterating over all usage mode values.
 *
 * Enumeration of PIN Policy Usage Modes.  This indicate the way the
 * PIN is used.
 */
typedef enum
{
  PSKC_PINUSAGEMODE_UNKNOWN = 0,
  PSKC_PINUSAGEMODE_LOCAL = 1,
  PSKC_PINUSAGEMODE_PREPEND = 2,
  PSKC_PINUSAGEMODE_APPEND = 3,
  PSKC_PINUSAGEMODE_ALGORITHMIC = 4,
  /* Make sure the following value is the highest. */
  PSKC_PINUSAGEMODE_LAST = PSKC_PINUSAGEMODE_ALGORITHMIC
} pskc_pinusagemode;

extern PSKCAPI const char *pskc_pinusagemode2str (pskc_pinusagemode
						  pinusagemode);
extern PSKCAPI pskc_pinusagemode pskc_str2pinusagemode (const char
							*pinusagemode);

/**
 * pskc_valueformat:
 * @PSKC_VALUEFORMAT_UNKNOWN: Unknown format.
 * @PSKC_VALUEFORMAT_DECIMAL: Only numerical digits.
 * @PSKC_VALUEFORMAT_HEXADECIMAL: Hexadecimal response.
 * @PSKC_VALUEFORMAT_ALPHANUMERIC: All letters and numbers (case sensitive).
 * @PSKC_VALUEFORMAT_BASE64: Base-64 encoded.
 * @PSKC_VALUEFORMAT_BINARY: Binary data.
 * @PSKC_VALUEFORMAT_LAST: Meta-value corresponding to the highest value,
 * for use in iterating over all encoding format values.
 *
 * Enumeration of PSKC value encoding formats.
 */
typedef enum
{
  PSKC_VALUEFORMAT_UNKNOWN = 0,
  PSKC_VALUEFORMAT_DECIMAL = 1,
  PSKC_VALUEFORMAT_HEXADECIMAL = 2,
  PSKC_VALUEFORMAT_ALPHANUMERIC = 3,
  PSKC_VALUEFORMAT_BASE64 = 4,
  PSKC_VALUEFORMAT_BINARY = 5,
  /* Make sure the following value is the highest. */
  PSKC_VALUEFORMAT_LAST = PSKC_VALUEFORMAT_BINARY
} pskc_valueformat;

extern PSKCAPI const char *pskc_valueformat2str (pskc_valueformat
						 valueformat);
extern PSKCAPI pskc_valueformat pskc_str2valueformat (const char
						      *valueformat);

/**
 * pskc_keyusage:
 * @PSKC_KEYUSAGE_UNKNOWN: Unknown format.
 * @PSKC_KEYUSAGE_OTP: The key MUST only be used for OTP generation.
 * @PSKC_KEYUSAGE_CR: The key MUST only be used for Challenge/Response purposes.
 * @PSKC_KEYUSAGE_ENCRYPT: The key MUST only be used for data
 * encryption purposes.
 * @PSKC_KEYUSAGE_INTEGRITY: The key MUST only be used to generate a
 * keyed message digest for data integrity or authentication purposes.
 * @PSKC_KEYUSAGE_VERIFY: The key MUST only be used to verify a keyed
 * message digest for data integrity or authentication purposes (this
 * is the opposite key usage of 'Integrity').
 * @PSKC_KEYUSAGE_UNLOCK: The key MUST only be used for an inverse
 * Challenge/ Response in the case where a user has locked the device
 * by entering a wrong PIN too many times (for devices with PIN-input
 * capability).
 * @PSKC_KEYUSAGE_DECRYPT: The key MUST only be used for data
 * decryption purposes.
 * @PSKC_KEYUSAGE_KEYWRAP: The key MUST only be used for key wrap purposes.
 * @PSKC_KEYUSAGE_UNWRAP: The key MUST only be used for key unwrap purposes.
 * @PSKC_KEYUSAGE_DERIVE: The key MUST only be used with a key
 * derivation function to derive a new key.
 * @PSKC_KEYUSAGE_GENERATE: The key MUST only be used to generate a
 * new key based on a random number and the previous value of the key.
 * @PSKC_KEYUSAGE_LAST: Meta-value corresponding to the highest value,
 * for use in iterating over all key usage values.
 *
 * Enumeration of PSKC key usage values.  These values puts
 * constraints on the intended usage of the key.  The recipient of the
 * PSKC document MUST enforce the key usage.  The values are assigned
 * to numbers so that they can be ORed together to form a set of
 * values.
 */
typedef enum
{
  PSKC_KEYUSAGE_UNKNOWN = 0,
  PSKC_KEYUSAGE_OTP = 1,
  PSKC_KEYUSAGE_CR = 2,
  PSKC_KEYUSAGE_ENCRYPT = 4,
  PSKC_KEYUSAGE_INTEGRITY = 8,
  PSKC_KEYUSAGE_VERIFY = 16,
  PSKC_KEYUSAGE_UNLOCK = 32,
  PSKC_KEYUSAGE_DECRYPT = 64,
  PSKC_KEYUSAGE_KEYWRAP = 128,
  PSKC_KEYUSAGE_UNWRAP = 256,
  PSKC_KEYUSAGE_DERIVE = 512,
  PSKC_KEYUSAGE_GENERATE = 1024,
  /* Make sure the following value is the highest. */
  PSKC_KEYUSAGE_LAST = PSKC_KEYUSAGE_GENERATE
} pskc_keyusage;

extern PSKCAPI const char *pskc_keyusage2str (pskc_keyusage keyusage);
extern PSKCAPI pskc_keyusage pskc_str2keyusage (const char *keyusage);

#endif /* PSKC_ENUMS_H */
