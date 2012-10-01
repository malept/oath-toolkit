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

#include "internal.h"
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memcpy */
#include "inttostr.h"		/* umaxtostr */

static int
build_deviceinfo (pskc_key_t *kp, xmlNodePtr keyp)
{
  const char *device_manufacturer = pskc_get_device_manufacturer (kp);
  const char *device_serialno = pskc_get_device_serialno (kp);
  const char *device_userid = pskc_get_device_userid (kp);
  xmlNodePtr devinfo;

  if (!device_manufacturer && !device_serialno && !device_userid)
    return PSKC_OK;

  devinfo = xmlNewChild (keyp, NULL, BAD_CAST "DeviceInfo", NULL);

  if (device_manufacturer)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "Manufacturer",
		     BAD_CAST device_manufacturer);

  if (device_serialno)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "SerialNo",
		     BAD_CAST device_serialno);

  /* XXX */

  if (device_userid)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "UserId",
		     BAD_CAST device_userid);

  return PSKC_OK;
}

static int
build_cryptomoduleinfo (pskc_key_t *kp, xmlNodePtr keyp)
{
  const char *cryptomodule_id = pskc_get_cryptomodule_id (kp);
  xmlNodePtr cminfo;

  if (!cryptomodule_id)
    return PSKC_OK;

  cminfo = xmlNewChild (keyp, NULL, BAD_CAST "CryptoModuleInfo", NULL);

  if (cryptomodule_id)
    xmlNewTextChild (cminfo, NULL, BAD_CAST "Id",
		     BAD_CAST cryptomodule_id);

  return PSKC_OK;
}

static int
build_algparm (pskc_key_t *kp, xmlNodePtr keyp)
{
  const char *algparm_suite = pskc_get_key_algparm_suite (kp);
  int algparm_resp_encoding_p;
  pskc_valueformat algparm_resp_encoding =
    pskc_get_key_algparm_resp_encoding (kp, &algparm_resp_encoding_p);
  const char *algparm_resp_encoding_str =
    pskc_valueformat2str (algparm_resp_encoding);
  int algparm_resp_length_p;
  uint32_t algparm_resp_length =
    pskc_get_key_algparm_resp_length (kp, &algparm_resp_length_p);
  xmlNodePtr algparm, rspfmt;

  if (!algparm_suite && !algparm_resp_encoding_p && !algparm_resp_length_p)
    return PSKC_OK;

  algparm = xmlNewChild (keyp, NULL, BAD_CAST "AlgorithmParameters", NULL);

  if (algparm_suite)
    xmlNewTextChild (algparm, NULL, BAD_CAST "Suite",
		     BAD_CAST algparm_suite);

  rspfmt = xmlNewChild (algparm, NULL, BAD_CAST "ResponseFormat", NULL);

  if (algparm_resp_length_p)
    {
      char buf[INT_BUFSIZE_BOUND(uintmax_t)];
      char *p = umaxtostr (algparm_resp_length, buf);
      xmlNewProp (rspfmt, BAD_CAST "Length", BAD_CAST p);
    }

  if (algparm_resp_encoding_p)
    xmlNewProp (rspfmt, BAD_CAST "Encoding",
		BAD_CAST algparm_resp_encoding_str);

  return PSKC_OK;
}

static int
build_policy (pskc_key_t *kp, xmlNodePtr keyp)
{
  int keyusage_p;
  pskc_keyusage keyusage = pskc_get_key_policy_keyusage (kp, &keyusage_p);
  const char *keyusage_str = pskc_keyusage2str (keyusage);
  const struct tm *startdate = pskc_get_key_policy_startdate (kp);
  const struct tm *expirydate = pskc_get_key_policy_expirydate (kp);
  const char *pinkeyid = pskc_get_key_policy_pinkeyid (kp);
  int pinusagemode_p;
  pskc_pinusagemode pinusagemode =
    pskc_get_key_policy_pinusagemode (kp, &pinusagemode_p);
  int attempts_p;
  uint32_t attempts =
    pskc_get_key_policy_pinmaxfailedattempts (kp, &attempts_p);
  int pinmin_p;
  uint32_t pinmin = pskc_get_key_policy_pinminlength (kp, &pinmin_p);
  int pinmax_p;
  uint32_t pinmax = pskc_get_key_policy_pinmaxlength (kp, &pinmax_p);
  int pinencoding_p;
  pskc_valueformat pinencoding =
    pskc_get_key_policy_pinencoding (kp, &pinencoding_p);
  xmlNodePtr policy, pinpolicy;

  if (!keyusage_p && !startdate && !expirydate && !pinkeyid && !pinusagemode_p
      && !attempts_p && !pinmin_p && !pinmax_p && !pinencoding_p)
    return PSKC_OK;

  policy = xmlNewChild (keyp, NULL, BAD_CAST "Policy", NULL);

  if (startdate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%dT%H:%M:%SZ", startdate);
      xmlNewTextChild (policy, NULL, BAD_CAST "StartDate", BAD_CAST t);
    }

  if (expirydate)
    {
      char t[100];
      strftime (t, sizeof (t), "%Y-%m-%dT%H:%M:%SZ", expirydate);
      xmlNewTextChild (policy, NULL, BAD_CAST "ExpiryDate", BAD_CAST t);
    }

  if (pinkeyid || pinusagemode_p || attempts_p
      || pinmin_p || pinmax_p || pinencoding_p)
    {
      pinpolicy = xmlNewChild (policy, NULL, BAD_CAST "PINPolicy", NULL);

      if (pinkeyid)
	xmlNewProp (pinpolicy, BAD_CAST "PINKeyId", BAD_CAST pinkeyid);

      if (pinusagemode_p)
	xmlNewProp (pinpolicy, BAD_CAST "PINUsageMode",
		    BAD_CAST pskc_pinusagemode2str (pinusagemode));

      if (attempts_p)
	{
	  char buf[INT_BUFSIZE_BOUND(uintmax_t)];
	  char *p = umaxtostr (attempts, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MaxFailedAttempts", BAD_CAST p);
	}

      if (pinmin_p)
	{
	  char buf[INT_BUFSIZE_BOUND(uintmax_t)];
	  char *p = umaxtostr (pinmin, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MinLength", BAD_CAST p);
	}

      if (pinmax_p)
	{
	  char buf[INT_BUFSIZE_BOUND(uintmax_t)];
	  char *p = umaxtostr (pinmax, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MaxLength", BAD_CAST p);
	}

      if (pinencoding_p)
	xmlNewProp (pinpolicy, BAD_CAST "PINEncoding",
		    BAD_CAST pskc_valueformat2str (pinencoding));
    }

  if (keyusage_p)
    xmlNewTextChild (policy, NULL, BAD_CAST "KeyUsage", BAD_CAST keyusage_str);

  return PSKC_OK;
}

static int
build_key (pskc_key_t *kp, xmlNodePtr keyp)
{
  const char *id = pskc_get_key_id (kp);
  const char *alg = pskc_get_key_algorithm (kp);
  const char *issuer = pskc_get_key_issuer (kp);
  const char *userid = pskc_get_key_userid (kp);
  xmlNodePtr key;

  if (!id && !alg && !issuer && !userid)
    return PSKC_OK;

  key = xmlNewChild (keyp, NULL, BAD_CAST "Key", NULL);

  if (id)
    xmlNewProp (key, BAD_CAST "Id", BAD_CAST id);

  if (alg)
    xmlNewProp (key, BAD_CAST "Algorithm", BAD_CAST alg);

  if (issuer)
    xmlNewTextChild (key, NULL, BAD_CAST "Issuer",
		     BAD_CAST issuer);

  build_algparm (kp, key);

  if (userid)
    xmlNewTextChild (key, NULL, BAD_CAST "UserId",
		     BAD_CAST userid);

  build_policy (kp, key);

  return PSKC_OK;
}

static int
build_keypackage (pskc_key_t *kp, xmlNodePtr keyp)
{
  build_deviceinfo (kp, keyp);
  build_cryptomoduleinfo (kp, keyp);
  build_key (kp, keyp);

  return PSKC_OK;
}

static int
build_keycont (pskc_t *container, xmlNodePtr keycont)
{
  xmlNodePtr keypackage;
  const char *ver = pskc_get_version (container);;
  const char *id = pskc_get_id (container);
  xmlChar *pskcver = BAD_CAST (ver ? ver : "1.0");
  pskc_key_t *kp;
  size_t i;

  if (xmlNewNs (keycont, BAD_CAST "urn:ietf:params:xml:ns:keyprov:pskc",
		NULL) == NULL)
    return PSKC_XML_ERROR;

  if (xmlNewProp (keycont, BAD_CAST "Version", pskcver) == NULL)
    return PSKC_XML_ERROR;

  if (id && xmlNewProp (keycont, BAD_CAST "Id", BAD_CAST id) == NULL)
    return PSKC_XML_ERROR;

  for (i = 0; (kp = pskc_get_keypackage (container, i)); i++)
    {
      //xmlNewChild (keycont, NULL, BAD_CAST "KeyPackage", NULL);
      keypackage = xmlNewChild (keycont, NULL, BAD_CAST "KeyPackage", NULL);
      build_keypackage (kp, keypackage);
    }

  return PSKC_OK;
}

/**
 * pskc_build_xml:
 * @container: pointer to #pskc_t handle
 * @out: pointer to output variable to hold newly allocated string
 * @len: output variable holding length of *@out.
 *
 * This function builds a XML file from the data in @container and
 * converts it to a string placed in the newly allocated *@out of
 * length @len.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned.
 **/
int
pskc_build_xml (pskc_t *container, char **out, size_t *len)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr keycont = NULL;
  xmlChar *mem = NULL;
  int buffersize = 0;
  int rc;

  doc = xmlNewDoc (BAD_CAST "1.0");
  if (doc == NULL)
    return PSKC_XML_ERROR;

  keycont = xmlNewNode (NULL, BAD_CAST "KeyContainer");
  if (keycont == NULL)
    {
      rc = PSKC_XML_ERROR;
      goto done;
    }

  rc = build_keycont (container, keycont);
  if (rc != PSKC_OK)
    goto done;

  xmlDocSetRootElement (doc, keycont);

  xmlDocDumpFormatMemory (doc, &mem, &buffersize, 1);
  *len = buffersize;
  if (mem == NULL || buffersize <= 0)
    {
      rc = PSKC_XML_ERROR;
      goto done;
    }

  *len = buffersize;
  *out = malloc (buffersize);
  if (*out == NULL)
    {
      rc = PSKC_MALLOC_ERROR;
      goto done;
    }

  memcpy (*out, mem, buffersize);

  rc = PSKC_OK;

 done:
  if (mem)
    xmlFree (mem);
  if (doc)
    xmlFreeDoc (doc);
  return rc;
}
