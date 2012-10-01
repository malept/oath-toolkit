/*
 * container.c - implementation of PSKC container handling
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

#define INTERNAL_NEED_PSKC_STRUCT
#define INTERNAL_NEED_PSKC_KEY_STRUCT
#include "internal.h"
#include <stdlib.h>		/* malloc */
#include <string.h>		/* strcmp */

/**
 * pskc_init:
 * @container: pointer to #pskc_t handle to initialize
 *
 * This function initializes the PSKC @container handle.  The memory
 * allocate can be released by calling pskc_done().
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned.
 **/
int
pskc_init (pskc_t ** container)
{
  *container = calloc (1, sizeof (**container));
  if (*container == NULL)
    return PSKC_MALLOC_ERROR;
  return PSKC_OK;
}

/**
 * pskc_init_from_memory:
 * @container: pointer to #pskc_t handle to initialize
 * @len: length of @buffer.
 * @buffer: XML data to parse.
 *
 * This function initializes the PSKC @container handle.  The memory
 * allocate can be released by calling pskc_done().  The function will
 * also parse the XML data in @buffer of @len size.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned, on XML parse
 *   errors %PSKC_XML_PARSE_ERROR is returned.
 **/
int
pskc_init_from_memory (pskc_t ** container, size_t len, const char *buffer)
{
  xmlDocPtr xmldoc;
  int rc;

  rc = pskc_init (container);
  if (rc != PSKC_OK)
    return rc;

  xmldoc = xmlReadMemory (buffer, len, NULL, NULL, XML_PARSE_NONET);
  if (xmldoc == NULL)
    {
      pskc_done (*container);
      return PSKC_XML_PARSE_ERROR;
    }

  (*container)->xmldoc = xmldoc;

  rc = _pskc_parse (*container);
  if (rc != PSKC_OK)
    {
      pskc_done (*container);
      return rc;
    }

  return PSKC_OK;
}

/**
 * pskc_done:
 * @container: #pskc_t handle to deinitialize
 *
 * This function releases the resources associated with the PSKC
 * @container handle.
 **/
void
pskc_done (pskc_t * container)
{
  xmlFreeDoc (container->xmldoc);
  free (container->keypackages);
  free (container);
}

/**
 * pskc_get_version:
 * @container: #pskc_t handle
 *
 * Get the PSKC KeyContainer Version attribute.  Normally this string
 * is always "1.0" and a missing field is a syntax error according to
 * the PSKC schema.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_version (pskc_t *container)
{
  return container->version;
}

/**
 * pskc_get_id:
 * @container: #pskc_t handle
 *
 * Get the PSKC KeyContainer Id attribute.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_id (pskc_t *container)
{
  return container->id;
}

/**
 * pskc_get_keypackage:
 * @container: #pskc_t handle
 * @i: number of keypackage to get
 *
 * Get a PSKC keypackage #pskc_key_t handle for the @i'th key package
 * in @container.  @i is zero-based, i.e., 0 refer to the first key
 * package, 1 refer to the second key package, and so on.
 *
 * Returns: NULL if there is no @i'th key package, or a valid
 *   #pskc_key_t pointer.
 */
pskc_key_t *
pskc_get_keypackage (pskc_t *container,
		     size_t i)
{
  if (i >= container->nkeypackages)
    return NULL;
  return &container->keypackages[i];
}

/**
 * pskc_get_device_manufacturer:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo Manufacturer value.  This
 * element indicates the manufacturer of the device.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_manufacturer (pskc_key_t *key)
{
  return key->device_manufacturer;
}

/**
 * pskc_get_device_serialno:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo SerialNo value.  This element
 * contains the serial number of the device.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_serialno (pskc_key_t *key)
{
  return key->device_serialno;
}

/**
 * pskc_get_device_model:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo Model value.  This element
 * describes the model of the device (e.g.,
 * "one-button-HOTP-token-V1").
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_model (pskc_key_t *key)
{
  return key->device_model;
}

/**
 * pskc_get_device_issueno:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo Issueno value.  This element
 * contains the issue number in case there are devices with the same
 * serial number so that they can be distinguished by different issue
 * numbers.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_issueno (pskc_key_t *key)
{
  return key->device_issueno;
}

/**
 * pskc_get_device_devicebinding:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo Devicebinding value.  This
 * element allows a provisioning server to ensure that the key is
 * going to be loaded into the device for which the key provisioning
 * request was approved.  The device is bound to the request using a
 * device identifier, e.g., an International Mobile Equipment Identity
 * (IMEI) for the phone, or an identifier for a class of identifiers,
 * e.g., those for which the keys are protected by a Trusted Platform
 * Module (TPM).
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_devicebinding (pskc_key_t *key)
{
  return key->device_devicebinding;
}

/**
 * pskc_get_device_startdate:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo StartDate.  This element denote
 * the start date of a device (such as the one on a payment card, used
 * when issue numbers are not printed on cards).
 *
 * Returns: a constant struct (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const struct tm *
pskc_get_device_startdate (pskc_key_t *key)
{
  if (!key->device_startdate_str)
    return NULL;
  return &key->device_startdate;
}

/**
 * pskc_get_device_expirydate:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo ExpiryDate.  This element denote
 * the end date of a device (such as the one on a payment card, used
 * when issue numbers are not printed on cards).
 *
 * Returns: a constant struct (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const struct tm *
pskc_get_device_expirydate (pskc_key_t *key)
{
  if (!key->device_expirydate_str)
    return NULL;
  return &key->device_expirydate;
}

/**
 * pskc_get_device_userid:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage DeviceInfo Userid value.  This indicates
 * the user with whom the device is associated.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_device_userid (pskc_key_t *key)
{
  return key->device_userid;
}

/**
 * pskc_get_cryptomodule_id:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage CryptoModule Id value.  This element
 * carries a unique identifier for the CryptoModule and is
 * implementation specific.  As such, it helps to identify a specific
 * CryptoModule to which the key is being or was provisioned.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_cryptomodule_id (pskc_key_t *key)
{
  return key->cryptomodule_id;
}

/**
 * pskc_get_key_id:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Id attribute value.  It is a syntax
 * error for this attribute to not be available.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_id (pskc_key_t *key)
{
  return key->key_id;
}

/**
 * pskc_get_key_algorithm:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Algorithm attribute value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_algorithm (pskc_key_t *key)
{
  return key->key_algorithm;
}

/**
 * pskc_get_key_issuer:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Issuer value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_issuer (pskc_key_t *key)
{
  return key->key_issuer;
}

/**
 * pskc_get_key_algparm_suite:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key AlgorithmParameters Suite value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_algparm_suite (pskc_key_t *key)
{
  return key->key_algparm_suite;
}

/**
 * pskc_get_key_algparm_resp_encoding:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key AlgorithmParameters ResponseFormat
 * Encoding value.  This attribute defines the encoding of the
 * response generated by the device, it MUST be included and MUST be
 * one of the #pskc_valueformat values.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an #pskc_valueformat value
 */
pskc_valueformat
pskc_get_key_algparm_resp_encoding (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_algparm_resp_encoding_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_algparm_resp_encoding;
}

/**
 * pskc_get_key_algparm_resp_length:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key AlgorithmParameters ResponseFormat
 * Length value.  This attribute defines the length of the response
 * generated by the device and MUST be included.  If the 'Encoding'
 * attribute is set to 'DECIMAL', 'HEXADECIMAL', or ALPHANUMERIC, this
 * value indicates the number of digits/characters.  If the 'Encoding'
 * attribute is set to 'BASE64' or 'BINARY', this value indicates the
 * number of bytes of the unencoded value.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_algparm_resp_length (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_algparm_resp_length_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_algparm_resp_length;
}

/**
 * pskc_get_key_algparm_resp_checkdigits:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key AlgorithmParameters ResponseFormat
 * CheckDigits value.  This attribute indicates whether the device
 * needs to append a Luhn check digit, as defined in [ISOIEC7812], to
 * the response.  This is only valid if the 'Encoding' attribute is
 * set to 'DECIMAL'.  If the value is TRUE, then the device will
 * append a Luhn check digit to the response.  If the value is FALSE,
 * then the device will not append a Luhn check digit to the response.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: 1 to indicate a CheckDigits value of true, or 0 to
 * indicate false.
 */
int
pskc_get_key_algparm_resp_checkdigits (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_algparm_resp_checkdigits_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_algparm_resp_checkdigits;
}

/**
 * pskc_get_key_profileid:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key KeyProfileId value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_profileid (pskc_key_t *key)
{
  return key->key_profileid;
}

/**
 * pskc_get_key_reference:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key KeyReference value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_reference (pskc_key_t *key)
{
  return key->key_reference;
}

/**
 * pskc_get_key_friendlyname:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Friendlyname value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_friendlyname (pskc_key_t *key)
{
  return key->key_friendlyname;
}

/**
 * pskc_get_key_userid:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Userid value.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_userid (pskc_key_t *key)
{
  return key->key_userid;
}

/**
 * pskc_get_key_data_secret:
 * @key: #pskc_t handle.
 * @len: pointer to output variable with length of returned data.
 *
 * Get the PSKC KeyPackage Key Data Secret value.  If @len is not set,
 * the caller can only use the returned value for comparison against
 * NULL to check whether the field is present or not.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content of length *@len, or NULL if not set.
 */
const char *
pskc_get_key_data_secret (pskc_key_t *key,
			  size_t *len)
{
  if (len)
    *len = key->key_secret_len;
  return key->key_secret;
}

/**
 * pskc_get_key_data_counter:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Data Counter value.  This element
 * contains the event counter for event- based OTP algorithms.
 *
 * If @present is non-NULL, it will be 0 if the Counter field is not
 * present or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint64_t
pskc_get_key_data_counter (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_counter_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_counter;
}

/**
 * pskc_get_key_data_time:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Data Time value.  This element contains
 * the time for time-based OTP algorithms.  (If time intervals are
 * used, this element carries the number of time intervals passed from
 * a specific start point, normally it is algorithm dependent).
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_data_time (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_time_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_time;
}

/**
 * pskc_get_key_data_timeinterval:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Data TimeInterval value.  This element
 * carries the time interval value for time-based OTP algorithms in
 * seconds (a typical value for this would be 30, indicating a time
 * interval of 30 seconds).
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_data_timeinterval (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_timeinterval_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_timeinterval;
}

/**
 * pskc_get_key_data_timedrift:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Data TimeDrift value.  This element
 * contains the device clock drift value for time-based OTP
 * algorithms.  The integer value (positive or negative drift) that
 * indicates the number of time intervals that a validation server has
 * established the device clock drifted after the last successful
 * authentication.  So, for example, if the last successful
 * authentication established a device time value of 8 intervals from
 * a specific start date but the validation server determines the time
 * value at 9 intervals, the server SHOULD record the drift as -1.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_data_timedrift (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_timedrift_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_timedrift;
}

/**
 * pskc_get_key_policy_startdate:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Policy StartDate.  This element denote
 * the start of the validity period of a key.
 *
 * Returns: a constant struct (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const struct tm *
pskc_get_key_policy_startdate (pskc_key_t *key)
{
  if (!key->key_policy_startdate_str)
    return NULL;
  return &key->key_policy_startdate;
}

/**
 * pskc_get_key_policy_expirydate:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Policy ExpiryDate.  This element denote
 * the expiry of the validity period of a key.
 *
 * Returns: a constant struct (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const struct tm *
pskc_get_key_policy_expirydate (pskc_key_t *key)
{
  if (!key->key_policy_expirydate_str)
    return NULL;
  return &key->key_policy_expirydate;
}

/**
 * pskc_get_key_policy_pinkeyid:
 * @key: #pskc_t handle
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy PINKeyId value.  This
 * attribute carries the unique 'Id' attribute vale of the "Key"
 * element held within this "KeyContainer" that contains the value of
 * the PIN that protects the key.
 *
 * Returns: a constant string (must not be deallocated) holding the
 *   content, or NULL if not set.
 */
const char *
pskc_get_key_policy_pinkeyid (pskc_key_t *key)
{
  return key->key_policy_pinkeyid;
}

/**
 * pskc_get_key_policy_pinusagemode:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy PINUsageMode value.
 * This mandatory attribute indicates the way the PIN is used during
 * the usage of the key.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an #pskc_pinusagemode value
 */
pskc_pinusagemode
pskc_get_key_policy_pinusagemode (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_pinusagemode_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_pinusagemode;
}

/**
 * pskc_get_key_policy_pinmaxfailedattempts:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy MaxFailedAttempts
 * value.  This attribute indicates the maximum number of times the
 * PIN may be entered wrongly before it MUST NOT be possible to use
 * the key anymore (typical reasonable values are in the positive
 * integer range of at least 2 and no more than 10).
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_policy_pinmaxfailedattempts (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_pinmaxfailedattempts_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_pinmaxfailedattempts;
}

/**
 * pskc_get_key_policy_pinminlength:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy MinLength value.  This
 * attribute indicates the minimum length of a PIN that can be set to
 * protect the associated key.  It MUST NOT be possible to set a PIN
 * shorter than this value.  If the 'PINFormat' attribute is set to
 * 'DECIMAL', 'HEXADECIMAL', or 'ALPHANUMERIC', this value indicates
 * the number of digits/ characters.  If the 'PINFormat' attribute is
 * set to 'BASE64' or 'BINARY', this value indicates the number of
 * bytes of the unencoded value.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_policy_pinminlength (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_pinminlength_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_pinminlength;
}

/**
 * pskc_get_key_policy_pinmaxlength:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy MaxLength value.  This
 * attribute indicates the maximum length of a PIN that can be set to
 * protect this key.  It MUST NOT be possible to set a PIN longer than
 * this value.  If the 'PINFormat' attribute is set to 'DECIMAL',
 * 'HEXADECIMAL', or 'ALPHANUMERIC', this value indicates the number
 * of digits/ characters.  If the 'PINFormat' attribute is set to
 * 'BASE64' or 'BINARY', this value indicates the number of bytes of
 * the unencoded value.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint32_t
pskc_get_key_policy_pinmaxlength (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_pinmaxlength_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_pinmaxlength;
}

/**
 * pskc_get_key_policy_pinencoding:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy PINPolicy PINEncoding value.
 * This attribute indicates the encoding of the PIN and MUST be one of
 * the #pskc_valueformat values.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an #pskc_valueformat value
 */
pskc_valueformat
pskc_get_key_policy_pinencoding (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_pinencoding_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_pinencoding;
}

/**
 * pskc_get_key_policy_numberoftransactions:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy NumberOfTransactions value.  The
 * value in this element indicates the maximum number of times a key
 * carried within the PSKC document can be used by an application
 * after having received it.  When this element is omitted, there is
 * no restriction regarding the number of times a key can be used.
 *
 * Note that while the PSKC specification uses the XML data type
 * "nonNegativeInteger" for this variable, this implementation
 * restricts the size of the value to 64-bit integers.
 *
 * If @present is non-NULL, it will be 0 if the Counter field is not
 * present or 1 if it was present.
 *
 * Returns: an integer holding the content.
 */
uint64_t
pskc_get_key_policy_numberoftransactions (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_numberoftransactions_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_numberoftransactions;
}

/**
 * pskc_get_key_policy_keyusage:
 * @key: #pskc_t handle
 * @present: output variable indicating whether data was provided or not.
 *
 * Get the PSKC KeyPackage Key Policy KeyUsage value.  The element
 * puts constraints on the intended usage of the key.  The recipient
 * of the PSKC document MUST enforce the key usage.
 *
 * If @present is non-NULL, it will be 0 if the field is not present
 * or 1 if it was present.
 *
 * Returns: an #pskc_keyusage value
 */
pskc_keyusage
pskc_get_key_policy_keyusage (pskc_key_t *key, int *present)
{
  if (present)
    {
      if (key->key_policy_keyusage_str)
	*present = 1;
      else
	*present = 0;
    }

  return key->key_policy_keyusage;
}

/**
 * pskc_pinusagemode2str:
 * @pinusagemode: an #pskc_pinusagemode enumeration type
 *
 * Convert #pskc_pinusagemode to a string.  For example,
 * pskc_pinusagemode2str(%PSKC_PINUSAGEMODE_LOCAL) will return
 * "Local".  The returned string must not be deallocated.
 *
 * Returns: string corresponding to #pskc_pinusagemode.
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
 * Returns: string corresponding to #pskc_valueformat.
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
 * Returns: string corresponding to #pskc_keyusage.
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
      p = "Unknown";
      break;
    }

  return p;
}

/**
 * pskc_str2pinusagemode:
 * @valueformat: an string describing a key usage.
 *
 * Convert a string to a #pskc_pinusagemode type.  For example,
 * pskc_str2pinusagemode("Local") will return
 * %PSKC_PINUSAGEMODE_LOCAL.
 *
 * Returns: the corresponding #pskc_pinusagemode value.
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
 * Returns: the corresponding #pskc_valueformat value.
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
  return PSKC_VALUEFORMAT_UNKNOWN;
}

/**
 * pskc_str2keyusage:
 * @keyusage: an string describing a key usage.
 *
 * Convert a string to a #pskc_keyusage type.  For example,
 * pskc_str2keyusage("KeyWrap") will return %PSKC_KEYUSAGE_KEYWRAP.
 *
 * Returns: the corresponding #pskc_keyusage value.
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
  return PSKC_KEYUSAGE_UNKNOWN;
}
