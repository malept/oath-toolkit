/*
 * output.c - Pretty print PSKC data.
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

#include <config.h>

#include <pskc/pskc.h>

#include "minmax.h"

#define INTERNAL_NEED_PSKC_STRUCT
#include "internal.h"
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memmove */
#include <inttypes.h>		/* PRId64 */
#include <libxml/parser.h>	/* xmlInitParser */

struct buffer
{
  char *mem;			/* NULL if malloc has failed */
  size_t memlen;		/* length of allocated buffer */
  size_t dlen;			/* length of data in buffer */
};

#define CHUNK 1024

static void
buffer_init (struct buffer *buf)
{
  buf->memlen = CHUNK;
  buf->mem = malloc (buf->memlen);
  buf->mem[0] = '\0';
  buf->dlen = 0;
}

#if 0
static void
buffer_done (struct buffer *buf)
{
  if (buf)
    free (buf->mem);
  buffer_init (buf);
}
#endif
static void
buffer_getstr (struct buffer *buf, char **str, size_t * dlen)
{
  *str = buf->mem;
  *dlen = buf->dlen;
}

static void
buffer_add (struct buffer *buf, size_t len, const void *data)
{
  size_t n;

  if (len == 0 || buf->mem == NULL)
    return;

  n = buf->dlen + len;
  if (buf->memlen > n)
    {
      memmove (&buf->mem[buf->dlen], data, len);
      buf->dlen = n;
      buf->mem[buf->dlen] = '\0';
    }
  else
    {
      size_t newlen = buf->memlen + MAX (len, CHUNK);
      char *tmp;

      tmp = realloc (buf->mem, newlen);
      if (tmp == NULL)
	{
	  free (buf->mem);
	  buf->mem = NULL;
	  return;
	}
      buf->mem = tmp;
      buf->memlen = newlen;

      memmove (&buf->mem[buf->dlen], data, len);

      buf->dlen = n;
      buf->mem[buf->dlen] = '\0';
    }
}

static void
buffer_addz (struct buffer *buf, const char *str)
{
  buffer_add (buf, strlen (str), str);
}

static void
buffer_addf (struct buffer *buf, const char *fmt, ...)
__attribute__ ((format (printf, 2, 3)));

static void
buffer_addf (struct buffer *buf, const char *fmt, ...)
{
  va_list args;
  int len;
  char *str;

  va_start (args, fmt);
  len = vasprintf (&str, fmt, args);
  va_end (args);

  if (len < 0 || !str)
    {
      free (buf->mem);
      buf->mem = NULL;
      return;
    }

  buffer_addz (buf, str);

  free (str);
}

static void
print_keypackage (struct buffer *buf, pskc_key_t * kp)
{
  const char *device_manufacturer = pskc_get_device_manufacturer (kp);
  const char *device_serialno = pskc_get_device_serialno (kp);
  const char *device_model = pskc_get_device_model (kp);
  const char *device_issueno = pskc_get_device_issueno (kp);
  const char *device_devicebinding = pskc_get_device_devicebinding (kp);
  const struct tm *device_startdate = pskc_get_device_startdate (kp);
  const struct tm *device_expirydate = pskc_get_device_expirydate (kp);
  const char *device_userid = pskc_get_device_userid (kp);
  const char *cryptomodule_id = pskc_get_cryptomodule_id (kp);
  const char *key_id = pskc_get_key_id (kp);
  const char *key_friendlyname = pskc_get_key_friendlyname (kp);
  const char *key_issuer = pskc_get_key_issuer (kp);
  const char *key_profileid = pskc_get_key_profileid (kp);
  const char *key_reference = pskc_get_key_reference (kp);
  const char *key_userid = pskc_get_key_userid (kp);
  const char *key_algorithm = pskc_get_key_algorithm (kp);
  const char *key_b64secret = pskc_get_key_data_b64secret (kp);
  int key_counter_present;
  uint64_t key_counter = pskc_get_key_data_counter (kp, &key_counter_present);
  int key_time_present;
  uint32_t key_time = pskc_get_key_data_time (kp, &key_time_present);
  int key_timedrift_present;
  uint32_t key_timedrift = pskc_get_key_data_timedrift
    (kp, &key_timedrift_present);
  int key_timeinterval_present;
  uint32_t key_timeinterval = pskc_get_key_data_timeinterval
    (kp, &key_timeinterval_present);
  const char *key_policy_pinkeyid = pskc_get_key_policy_pinkeyid (kp);
  int key_policy_pinminlength_present;
  uint32_t key_policy_pinminlength = pskc_get_key_policy_pinminlength
    (kp, &key_policy_pinminlength_present);
  int key_policy_pinmaxlength_present;
  uint32_t key_policy_pinmaxlength = pskc_get_key_policy_pinmaxlength
    (kp, &key_policy_pinmaxlength_present);
  int key_policy_pinmaxfailedattempts_present;
  uint32_t key_policy_pinmaxfailedattempts =
    pskc_get_key_policy_pinmaxfailedattempts (kp,
					      &key_policy_pinmaxfailedattempts_present);
  int key_policy_pinusagemode_present;
  pskc_pinusagemode key_policy_pinusagemode =
    pskc_get_key_policy_pinusagemode (kp, &key_policy_pinusagemode_present);
  int key_policy_pinencoding_present;
  pskc_valueformat key_policy_pinencoding =
    pskc_get_key_policy_pinencoding (kp, &key_policy_pinencoding_present);
  int key_policy_keyusages_present;
  int key_policy_keyusages =
    pskc_get_key_policy_keyusages (kp, &key_policy_keyusages_present);
  const char *key_algparm_suite = pskc_get_key_algparm_suite (kp);
  int key_algparm_chall_encoding_present;
  pskc_valueformat key_algparm_chall_encoding =
    pskc_get_key_algparm_chall_encoding (kp,
					 &key_algparm_chall_encoding_present);
  int key_algparm_chall_min_present;
  uint32_t key_algparm_chall_min =
    pskc_get_key_algparm_chall_min (kp, &key_algparm_chall_min_present);
  int key_algparm_chall_max_present;
  uint32_t key_algparm_chall_max =
    pskc_get_key_algparm_chall_max (kp, &key_algparm_chall_max_present);
  int key_algparm_resp_encoding_present;
  pskc_valueformat key_algparm_resp_encoding =
    pskc_get_key_algparm_resp_encoding (kp,
					&key_algparm_resp_encoding_present);
  int key_algparm_resp_length_present;
  uint32_t key_algparm_resp_length =
    pskc_get_key_algparm_resp_length (kp, &key_algparm_resp_length_present);
  const struct tm *key_policy_startdate = pskc_get_key_policy_startdate (kp);
  const struct tm *key_policy_expirydate =
    pskc_get_key_policy_expirydate (kp);

  buffer_addz (buf, "\t\tDeviceInfo:\n");
  if (device_manufacturer)
    buffer_addf (buf, "\t\t\tManufacturer: %s\n", device_manufacturer);
  if (device_serialno)
    buffer_addf (buf, "\t\t\tSerialNo: %s\n", device_serialno);
  if (device_model)
    buffer_addf (buf, "\t\t\tModel: %s\n", device_model);
  if (device_issueno)
    buffer_addf (buf, "\t\t\tIssueNo: %s\n", device_issueno);
  if (device_devicebinding)
    buffer_addf (buf, "\t\t\tDeviceBinding: %s\n", device_devicebinding);
  if (device_startdate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%d %H:%M:%S", device_startdate);
      buffer_addf (buf, "\t\t\tDevice StartDate: %s\n", t);
    }
  if (device_expirydate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%d %H:%M:%S", device_expirydate);
      buffer_addf (buf, "\t\t\tDevice ExpiryDate: %s\n", t);
    }
  if (device_userid)
    buffer_addf (buf, "\t\t\tUserId: %s\n", device_userid);

  if (cryptomodule_id)
    buffer_addf (buf, "\t\tCryptoModuleInfo Id: %s\n", cryptomodule_id);

  buffer_addz (buf, "\t\tKey:\n");
  if (key_id)
    buffer_addf (buf, "\t\t\tId: %s\n", key_id);
  if (key_friendlyname)
    buffer_addf (buf, "\t\t\tFriendlyName: %s\n", key_friendlyname);
  if (key_issuer)
    buffer_addf (buf, "\t\t\tIssuer: %s\n", key_issuer);
  if (key_algorithm)
    buffer_addf (buf, "\t\t\tAlgorithm: %s\n", key_algorithm);
  if (key_userid)
    buffer_addf (buf, "\t\t\tKey User Id: %s\n", key_userid);
  if (key_profileid)
    buffer_addf (buf, "\t\t\tKey Profile Id: %s\n", key_profileid);
  if (key_reference)
    buffer_addf (buf, "\t\t\tKey Reference: %s\n", key_reference);
  if (key_b64secret)
    buffer_addf (buf, "\t\t\tKey Secret (base64): %s\n", key_b64secret);
  if (key_counter_present)
    buffer_addf (buf, "\t\t\tKey Counter: %" PRId64 "\n", key_counter);
  if (key_time_present)
    buffer_addf (buf, "\t\t\tKey Time: %" PRId32 "\n", key_time);
  if (key_timeinterval_present)
    buffer_addf (buf, "\t\t\tKey TimeInterval: %" PRId32 "\n",
		 key_timeinterval);
  if (key_timedrift_present)
    buffer_addf (buf, "\t\t\tKey TimeDrift: %" PRId32 "\n", key_timedrift);
  if (key_policy_keyusages_present)
    {
      int i;
      buffer_addf (buf, "\t\t\tKey Usage:");
      for (i = 1; i <= PSKC_KEYUSAGE_LAST; i = i << 1)
	{
	  const char *keyusage_str = pskc_keyusage2str (i);

	  if (key_policy_keyusages & i)
	    buffer_addf (buf, " %s", keyusage_str);
	}
      buffer_addf (buf, "\n");
    }
  if (key_policy_startdate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%d %H:%M:%S", key_policy_startdate);
      buffer_addf (buf, "\t\t\tPolicy StartDate: %s\n", t);
    }
  if (key_policy_expirydate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%d %H:%M:%S", key_policy_expirydate);
      buffer_addf (buf, "\t\t\tPolicy ExpiryDate: %s\n", t);
    }
  if (key_policy_pinminlength_present)
    buffer_addf (buf, "\t\t\tPIN Policy Minimum Length: %" PRId32 "\n",
		 key_policy_pinminlength);
  if (key_policy_pinmaxlength_present)
    buffer_addf (buf, "\t\t\tPIN Policy Maximum Length: %" PRId32 "\n",
		 key_policy_pinmaxlength);
  if (key_policy_pinkeyid)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Key Id: %s\n",
		 key_policy_pinkeyid);
  if (key_policy_pinencoding_present)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Encoding: %s\n",
		 pskc_valueformat2str (key_policy_pinencoding));
  if (key_policy_pinusagemode_present)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Usage Mode: %s\n",
		 pskc_pinusagemode2str (key_policy_pinusagemode));
  if (key_policy_pinmaxfailedattempts_present)
    buffer_addf (buf,
		 "\t\t\tPIN Policy PIN Max Failed Attempts: %" PRId32 "\n",
		 key_policy_pinmaxfailedattempts);
  if (key_algparm_suite)
    buffer_addf (buf, "\t\t\tAlgorithm Parameters Suite: %s\n",
		 key_algparm_suite);
  if (key_algparm_chall_encoding_present)
    buffer_addf (buf, "\t\t\tChallenge Format Encoding: %s\n",
		 pskc_valueformat2str (key_algparm_chall_encoding));
  if (key_algparm_chall_min_present)
    buffer_addf (buf, "\t\t\tChallenge Format Min: %" PRId32 "\n",
		 key_algparm_chall_min);
  if (key_algparm_chall_max_present)
    buffer_addf (buf, "\t\t\tChallenge Format Max: %" PRId32 "\n",
		 key_algparm_chall_max);
  if (key_algparm_resp_length_present)
    buffer_addf (buf, "\t\t\tResponse Format Length: %" PRId32 "\n",
		 key_algparm_resp_length);
  if (key_algparm_resp_encoding_present)
    buffer_addf (buf, "\t\t\tResponse Format Encoding: %s\n",
		 pskc_valueformat2str (key_algparm_resp_encoding));
}

static void
print_keycontainer (pskc_t * data, struct buffer *buf)
{
  const char *version = pskc_get_version (data);
  const char *id = pskc_get_id (data);
  int signed_p = pskc_get_signed_p (data);
  pskc_key_t *key;
  size_t i;

  if (version)
    buffer_addf (buf, "\tVersion: %s\n", version);
  if (id)
    buffer_addf (buf, "\tId: %s\n", id);
  buffer_addf (buf, "\tSigned: %s\n", signed_p ? "YES" : "NO");

  for (i = 0; (key = pskc_get_keypackage (data, i)); i++)
    {
      buffer_addf (buf, "\tKeyPackage %zu:\n", i);
      print_keypackage (buf, key);
    }
}

/**
 * pskc_output:
 * @container: a #pskc_t handle, from pskc_init().
 * @format: an #pskc_output_formats_t enumeration type indicating format.
 * @out: pointer to output variable holding newly allocated string.
 * @len: pointer to output variable hold length of *@out.
 *
 * Convert PSKC data to a serialized string of the indicated type.
 * This is usually used to convert the PSKC data to some human
 * readable form.
 *
 * Returns: %PSKC_OK on success, or an error code.
 */
int
pskc_output (pskc_t * container,
	     pskc_output_formats_t format, char **out, size_t * len)
{
  if (format == PSKC_OUTPUT_HUMAN_COMPLETE)
    {
      struct buffer buf;

      buffer_init (&buf);
      buffer_addz (&buf, "Portable Symmetric Key Container (PSKC):\n");

      print_keycontainer (container, &buf);

      buffer_getstr (&buf, out, len);
      if (*out == NULL)
	return PSKC_MALLOC_ERROR;
    }
  else if (format == PSKC_OUTPUT_XML || format == PSKC_OUTPUT_INDENTED_XML)
    {
      xmlChar *mem;
      int size;

      xmlDocDumpMemory (container->xmldoc, &mem, &size);

      if (format == PSKC_OUTPUT_INDENTED_XML)
	{
	  xmlDocPtr xmldoc;

	  xmldoc = xmlReadMemory ((const char *) mem, size, NULL, NULL,
				  XML_PARSE_NONET | XML_PARSE_NOBLANKS);
	  if (xmldoc == NULL)
	    return PSKC_XML_ERROR;

	  xmlFree (mem);

	  xmlDocDumpFormatMemory (xmldoc, &mem, &size, 1);

	  xmlFreeDoc (xmldoc);
	}

      if (mem == NULL || size <= 0)
	{
	  if (format == PSKC_OUTPUT_XML)
	    _pskc_debug ("xmlDocDumpMemory failed");
	  else
	    _pskc_debug ("xmlDocDumpFormatMemory failed");
	  return PSKC_XML_ERROR;
	}

      if (len)
	*len = size;
      if (out)
	{
	  *out = malloc (size);
	  if (*out == NULL)
	    return PSKC_MALLOC_ERROR;

	  memcpy (*out, mem, size);
	}

      xmlFree (mem);
    }
  else
    return PSKC_UNKNOWN_OUTPUT_FORMAT;

  return PSKC_OK;
}
