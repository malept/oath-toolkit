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
  char *serialno;
  char *manufacturer;
  char *device_userid;
  /* CryptoModuleInfo */
  char *crypto_id;
  /* Key */
  char *key_id;
  char *key_algorithm;
  char *key_userid;
  char *key_issuer;
  char *key_secret;
  char *key_counter;
  char *key_time;
  char *key_time_interval;
  char *key_alg_resp_length;
  char *key_alg_resp_encoding;
  char *key_policy_startdate;
  char *key_policy_expirydate;
};

struct pskc_data
{
  /* raw XML */
  xmlDocPtr xmldoc;
  /* parsed version */
  char *version;
  char *id;
  size_t nkeypackages;
  struct pskc_keypackage *keypackages;
};

int _pskc_parse (pskc_data *data);
