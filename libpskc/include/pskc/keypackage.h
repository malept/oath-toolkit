/*
 * pskc/keypackage.h - PSKC header file with keypackage function prototypes.
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

#ifndef PSKC_KEYPACKAGE_H
#define PSKC_KEYPACKAGE_H

extern PSKCAPI const char *pskc_get_device_manufacturer (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_device_serialno (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_device_model (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_device_issueno (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_device_devicebinding (pskc_key_t *key);
extern PSKCAPI const struct tm *
pskc_get_device_startdate (pskc_key_t *key);
extern PSKCAPI const struct tm *
pskc_get_device_expirydate (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_device_userid (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_cryptomodule_id (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_id (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_algorithm (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_issuer (pskc_key_t *key);
extern PSKCAPI const char *
pskc_get_key_algparm_suite (pskc_key_t *key);
extern PSKCAPI pskc_valueformat
pskc_get_key_algparm_chall_encoding (pskc_key_t *key, int *present);
extern PSKCAPI uint32_t
pskc_get_key_algparm_chall_min (pskc_key_t *key, int *present);
extern PSKCAPI uint32_t
pskc_get_key_algparm_chall_max (pskc_key_t *key, int *present);
extern PSKCAPI int pskc_get_key_algparm_chall_checkdigits (pskc_key_t *key,
							   int *present);
extern PSKCAPI pskc_valueformat
pskc_get_key_algparm_resp_encoding (pskc_key_t *key, int *present);
extern PSKCAPI uint32_t
pskc_get_key_algparm_resp_length (pskc_key_t *key, int *present);
extern PSKCAPI int pskc_get_key_algparm_resp_checkdigits (pskc_key_t *key,
							  int *present);
extern PSKCAPI const char *pskc_get_key_profileid (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_reference (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_friendlyname (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_userid (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_data_secret (pskc_key_t *key,
						     size_t *len);
extern PSKCAPI const char *pskc_get_key_data_b64secret (pskc_key_t *key);
extern PSKCAPI uint64_t pskc_get_key_data_counter (pskc_key_t *key,
						   int *present);
extern PSKCAPI uint32_t pskc_get_key_data_time (pskc_key_t *key,
						int *present);
extern PSKCAPI uint32_t pskc_get_key_data_timeinterval (pskc_key_t *key,
							int *present);
extern PSKCAPI uint32_t pskc_get_key_data_timedrift (pskc_key_t *key,
						     int *present);
extern PSKCAPI const struct tm *
pskc_get_key_policy_startdate (pskc_key_t *key);
extern PSKCAPI const struct tm *
pskc_get_key_policy_expirydate (pskc_key_t *key);
extern PSKCAPI const char *pskc_get_key_policy_pinkeyid (pskc_key_t *key);
extern PSKCAPI pskc_pinusagemode
pskc_get_key_policy_pinusagemode (pskc_key_t *key, int *present);
extern PSKCAPI uint32_t
pskc_get_key_policy_pinmaxfailedattempts (pskc_key_t *key, int *present);
extern PSKCAPI uint32_t pskc_get_key_policy_pinminlength (pskc_key_t *key,
							  int *present);
extern PSKCAPI uint32_t pskc_get_key_policy_pinmaxlength (pskc_key_t *key,
							  int *present);
extern PSKCAPI pskc_valueformat
pskc_get_key_policy_pinencoding (pskc_key_t *key, int *present);
extern PSKCAPI int
pskc_get_key_policy_keyusages (pskc_key_t *key, int *present);
extern PSKCAPI uint64_t
pskc_get_key_policy_numberoftransactions (pskc_key_t *key, int *present);

#endif /* PSKC_KEYPACKAGE_H */
