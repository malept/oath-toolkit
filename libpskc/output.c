/*
 * output.c - pretty printing of PSKC data
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

#include "minmax.h"

#include "internal.h"
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memmove */
#include <libxml/parser.h>	/* xmlInitParser */

struct buffer
{
  char *mem; /* NULL if malloc has failed */
  size_t memlen; /* length of allocated buffer */
  size_t dlen; /* length of data in buffer */
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
buffer_getstr (struct buffer *buf, char **str, size_t *dlen)
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
      size_t newlen = buf->memlen + MAX(len, CHUNK);
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
print_keypackage (struct buffer *buf, struct pskc_keypackage *kp)
{
  buffer_addz (buf, "\t\tDeviceInfo:\n");
  if (kp->manufacturer)
    buffer_addf (buf, "\t\t\tManufacturer: %s\n", kp->manufacturer);
  if (kp->serialno)
    buffer_addf (buf, "\t\t\tSerialNo: %s\n", kp->serialno);
  if (kp->device_userid)
    buffer_addf (buf, "\t\t\tUserId: %s\n", kp->device_userid);

  if (kp->crypto_id)
    buffer_addf (buf, "\t\tCryptoModuleInfo Id: %s\n", kp->crypto_id);

  buffer_addz (buf, "\t\tKey:\n");
  if (kp->key_id)
    buffer_addf (buf, "\t\t\tId: %s\n", kp->key_id);
  if (kp->key_issuer)
    buffer_addf (buf, "\t\t\tIssuer: %s\n", kp->key_issuer);
  if (kp->key_profileid)
    buffer_addf (buf, "\t\t\tKey Profile Id: %s\n", kp->key_profileid);
  if (kp->key_reference)
    buffer_addf (buf, "\t\t\tKey Reference: %s\n", kp->key_reference);
  if (kp->key_userid)
    buffer_addf (buf, "\t\t\tKey User Id: %s\n", kp->key_userid);
  if (kp->key_algorithm)
    buffer_addf (buf, "\t\t\tAlgorithm: %s\n", kp->key_algorithm);
  if (kp->key_alg_resp_length)
    buffer_addf (buf, "\t\t\tResponseFormat Length: %s\n",
		 kp->key_alg_resp_length);
  if (kp->key_alg_resp_encoding)
    buffer_addf (buf, "\t\t\tResponseFormat Encoding: %s\n",
		 kp->key_alg_resp_encoding);
  if (kp->key_secret)
    buffer_addf (buf, "\t\t\tKey Secret: %s\n", kp->key_secret);
  if (kp->key_counter)
    buffer_addf (buf, "\t\t\tKey Counter: %s\n", kp->key_counter);
  if (kp->key_time)
    buffer_addf (buf, "\t\t\tKey Time: %s\n", kp->key_time);
  if (kp->key_time_interval)
    buffer_addf (buf, "\t\t\tKey Time Interval: %s\n", kp->key_time_interval);
  if (kp->key_usage)
    buffer_addf (buf, "\t\t\tKey Usage: %s\n", kp->key_usage);
  if (kp->key_policy_startdate)
    buffer_addf (buf, "\t\t\tPolicy StartDate: %s\n",
		 kp->key_policy_startdate);
  if (kp->key_policy_expirydate)
    buffer_addf (buf, "\t\t\tPolicy ExpiryDate: %s\n",
		 kp->key_policy_expirydate);
  if (kp->key_pinpolicy_minlength)
    buffer_addf (buf, "\t\t\tPIN Policy Minimum Length: %s\n",
		 kp->key_pinpolicy_minlength);
  if (kp->key_pinpolicy_maxlength)
    buffer_addf (buf, "\t\t\tPIN Policy Maximum Length: %s\n",
		 kp->key_pinpolicy_maxlength);
  if (kp->key_pinpolicy_pinkeyid)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Key Id: %s\n",
		 kp->key_pinpolicy_pinkeyid);
  if (kp->key_pinpolicy_pinencoding)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Encoding: %s\n",
		 kp->key_pinpolicy_pinencoding);
  if (kp->key_pinpolicy_pinusagemode)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Usage Mode: %s\n",
		 kp->key_pinpolicy_pinusagemode);
  if (kp->key_pinpolicy_maxfailedattempts)
    buffer_addf (buf, "\t\t\tPIN Policy PIN Max Failed Attempts: %s\n",
		 kp->key_pinpolicy_maxfailedattempts);
}

static void
print_keycontainer (pskc *data, struct buffer *buf)
{
  size_t i;

  buffer_addf (buf, "\tVersion: %s\n", data->version);
  if (data->id)
    buffer_addf (buf, "\tId: %s\n", data->id);

  for (i = 0; i < data->nkeypackages; i++)
    {
      buffer_addf (buf, "\tKeyPackage %zu:\n", i);
      print_keypackage (buf, &data->keypackages[i]);
    }
}

int
pskc_output (pskc *container,
	     pskc_output_formats_t format,
	     char **out, size_t *len)
{
  struct buffer buf;

  if (format != PSKC_OUTPUT_HUMAN_COMPLETE)
    return PSKC_UNKNOWN_OUTPUT_FORMAT;

  buffer_init (&buf);
  buffer_addz (&buf, "Portable Symmetric Key Container (PSKC):\n");

  print_keycontainer (container, &buf);

  buffer_getstr (&buf, out, len);
  if (*out == NULL)
    return PSKC_MALLOC_ERROR;

  return PSKC_OK;
}
