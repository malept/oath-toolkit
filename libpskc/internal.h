/*
 * internal.h - Internal prototypes.
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

#ifdef INTERNAL_NEED_PSKC_KEY_STRUCT
struct pskc_key
{
  /* Allocated by us */
  char *key_b64secret;
  char *key_secret;
  size_t key_secret_len;

  /* The rest are pointers into libxml structures. */

  /* DeviceInfo */
  const char *device_manufacturer;
  const char *device_serialno;
  const char *device_model;
  const char *device_issueno;
  const char *device_devicebinding;
  const char *device_startdate_str;
  struct tm device_startdate;
  const char *device_expirydate_str;
  struct tm device_expirydate;
  const char *device_userid;

  /* CryptoModuleInfo */
  const char *cryptomodule_id;

  /* Key */
  const char *key_id;
  const char *key_algorithm;
  const char *key_userid;
  const char *key_issuer;
  const char *key_secret_str;
  const char *key_counter_str;
  uint64_t key_counter;
  const char *key_time_str;
  uint32_t key_time;
  const char *key_timeinterval_str;
  uint32_t key_timeinterval;
  const char *key_timedrift_str;
  uint32_t key_timedrift;
  const char *key_friendlyname;
  const char *key_profileid;
  const char *key_reference;
  const char *key_policy_keyusage_str;
  int key_policy_keyusages;
  const char *key_policy_numberoftransactions_str;
  uint64_t key_policy_numberoftransactions;
  const char *key_algparm_suite;
  const char *key_algparm_chall_encoding_str;
  pskc_valueformat key_algparm_chall_encoding;
  const char *key_algparm_chall_min_str;
  uint32_t key_algparm_chall_min;
  const char *key_algparm_chall_max_str;
  uint32_t key_algparm_chall_max;
  const char *key_algparm_chall_checkdigits_str;
  int key_algparm_chall_checkdigits;
  const char *key_algparm_resp_encoding_str;
  pskc_valueformat key_algparm_resp_encoding;
  const char *key_algparm_resp_length_str;
  uint32_t key_algparm_resp_length;
  const char *key_algparm_resp_checkdigits_str;
  int key_algparm_resp_checkdigits;
  const char *key_policy_startdate_str;
  struct tm key_policy_startdate;
  const char *key_policy_expirydate_str;
  struct tm key_policy_expirydate;
  const char *key_policy_pinmaxfailedattempts_str;
  uint32_t key_policy_pinmaxfailedattempts;
  const char *key_policy_pinminlength_str;
  uint32_t key_policy_pinminlength;
  const char *key_policy_pinmaxlength_str;
  uint32_t key_policy_pinmaxlength;
  const char *key_policy_pinkeyid;
  const char *key_policy_pinusagemode_str;
  pskc_pinusagemode key_policy_pinusagemode;
  const char *key_policy_pinencoding_str;
  pskc_valueformat key_policy_pinencoding;
};
#endif

#ifdef INTERNAL_NEED_PSKC_STRUCT
#include <libxml/parser.h>
struct pskc
{
  /* raw XML */
  xmlDocPtr xmldoc;
  /* Is there a Signature element in xmldoc? */
  int signed_p;

  /* parsed data */
  const char *version;
  const char *id;
  size_t nkeypackages;
  struct pskc_key *keypackages;
};
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define _GL_ATTRIBUTE_FORMAT(spec) __attribute__ ((__format__ spec))
#else
#define _GL_ATTRIBUTE_FORMAT(spec)	/* empty */
#endif

extern void
_pskc_debug (const char *format, ...)
_GL_ATTRIBUTE_FORMAT ((printf, 1, 2));
