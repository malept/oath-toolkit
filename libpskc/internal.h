/*
 * internal.h - internal prototypes
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

#include <libxml/parser.h>

struct pskc_keypackage
{
  /* DeviceInfo */
  const char *serialno;
  const char *manufacturer;
  const char *device_userid;
  /* CryptoModuleInfo */
  const char *crypto_id;
  /* Key */
  const char *key_id;
  const char *key_algorithm;
  const char *key_userid;
  const char *key_issuer;
  const char *key_secret;
  const char *key_counter;
  const char *key_time;
  const char *key_time_interval;
  const char *key_alg_resp_length;
  const char *key_alg_resp_encoding;
  const char *key_profileid;
  const char *key_reference;
  const char *key_usage;
  const char *key_policy_startdate;
  const char *key_policy_expirydate;
  const char *key_pinpolicy_minlength;
  const char *key_pinpolicy_maxlength;
  const char *key_pinpolicy_pinkeyid;
  const char *key_pinpolicy_pinencoding;
  const char *key_pinpolicy_pinusagemode;
  const char *key_pinpolicy_maxfailedattempts;
};

struct pskc
{
  /* raw XML */
  xmlDocPtr xmldoc;
  /* parsed version */
  const char *version;
  const char *id;
  size_t nkeypackages;
  struct pskc_keypackage *keypackages;
};

int _pskc_parse (pskc *container);
