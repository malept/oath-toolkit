/*
 * build.c - create PSKC data.
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

#define INTERNAL_NEED_PSKC_STRUCT
#include "internal.h"
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memcpy */
#include <libxml/tree.h>	/* xmlNewChild */
#include "inttostr.h"		/* umaxtostr */

static int
build_deviceinfo (pskc_key_t * kp, xmlNodePtr keyp)
{
  const char *manufacturer = pskc_get_device_manufacturer (kp);
  const char *serialno = pskc_get_device_serialno (kp);
  const char *model = pskc_get_device_model (kp);
  const char *issueno = pskc_get_device_issueno (kp);
  const char *devicebinding = pskc_get_device_devicebinding (kp);
  const struct tm *startdate = pskc_get_device_startdate (kp);
  const struct tm *expirydate = pskc_get_device_expirydate (kp);
  const char *userid = pskc_get_device_userid (kp);
  xmlNodePtr devinfo;
  char t[100];

  if (!manufacturer && !serialno && !model && !issueno && !devicebinding
      && !startdate && !expirydate && !userid)
    return PSKC_OK;

  devinfo = xmlNewChild (keyp, NULL, BAD_CAST "DeviceInfo", NULL);

  if (manufacturer)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "Manufacturer",
		     BAD_CAST manufacturer);

  if (serialno)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "SerialNo", BAD_CAST serialno);

  if (model)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "Model", BAD_CAST model);

  if (issueno)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "IssueNo", BAD_CAST issueno);

  if (devicebinding)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "DeviceBinding",
		     BAD_CAST devicebinding);

  if (startdate)
    {
      strftime (t, sizeof (t), "%Y-%m-%dT%H:%M:%SZ", startdate);
      xmlNewTextChild (devinfo, NULL, BAD_CAST "StartDate", BAD_CAST t);
    }

  if (expirydate)
    {
      strftime (t, sizeof (t), "%Y-%m-%dT%H:%M:%SZ", expirydate);
      xmlNewTextChild (devinfo, NULL, BAD_CAST "ExpiryDate", BAD_CAST t);
    }

  if (userid)
    xmlNewTextChild (devinfo, NULL, BAD_CAST "UserId", BAD_CAST userid);

  return PSKC_OK;
}

static int
build_cryptomoduleinfo (pskc_key_t * kp, xmlNodePtr keyp)
{
  const char *cryptomodule_id = pskc_get_cryptomodule_id (kp);
  xmlNodePtr cminfo;

  if (!cryptomodule_id)
    return PSKC_OK;

  cminfo = xmlNewChild (keyp, NULL, BAD_CAST "CryptoModuleInfo", NULL);

  if (cryptomodule_id)
    xmlNewTextChild (cminfo, NULL, BAD_CAST "Id", BAD_CAST cryptomodule_id);

  return PSKC_OK;
}

static int
build_algparm (pskc_key_t * kp, xmlNodePtr keyp)
{
  const char *suite = pskc_get_key_algparm_suite (kp);
  int chall_encoding_p;
  pskc_valueformat chall_encoding = pskc_get_key_algparm_chall_encoding
    (kp, &chall_encoding_p);
  int chall_min_p;
  uint32_t chall_min = pskc_get_key_algparm_chall_min (kp, &chall_min_p);
  int chall_max_p;
  uint32_t chall_max = pskc_get_key_algparm_chall_max (kp, &chall_max_p);
  int chall_checkdigits_p;
  int chall_checkdigits = pskc_get_key_algparm_chall_checkdigits
    (kp, &chall_checkdigits_p);
  int resp_encoding_p;
  pskc_valueformat resp_encoding = pskc_get_key_algparm_resp_encoding
    (kp, &resp_encoding_p);
  int resp_length_p;
  uint32_t resp_length =
    pskc_get_key_algparm_resp_length (kp, &resp_length_p);
  int resp_checkdigits_p;
  int resp_checkdigits = pskc_get_key_algparm_resp_checkdigits
    (kp, &resp_checkdigits_p);
  xmlNodePtr algparm;

  if (!suite && !chall_encoding_p && !chall_min_p && !chall_max_p
      && !chall_checkdigits_p && !resp_encoding_p && !resp_length_p
      && !resp_checkdigits_p)
    return PSKC_OK;

  algparm = xmlNewChild (keyp, NULL, BAD_CAST "AlgorithmParameters", NULL);

  if (suite)
    xmlNewTextChild (algparm, NULL, BAD_CAST "Suite", BAD_CAST suite);

  if (chall_encoding_p || chall_min_p || chall_max_p || resp_checkdigits_p)
    {
      xmlNodePtr chall;

      chall = xmlNewChild (algparm, NULL, BAD_CAST "ChallengeFormat", NULL);

      if (chall_encoding_p)
	xmlNewProp (chall, BAD_CAST "Encoding",
		    BAD_CAST pskc_valueformat2str (chall_encoding));


      if (chall_min_p)
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (chall_min, buf);
	  xmlNewProp (chall, BAD_CAST "Min", BAD_CAST p);
	}

      if (chall_max_p)
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (chall_max, buf);
	  xmlNewProp (chall, BAD_CAST "Max", BAD_CAST p);
	}

      if (chall_checkdigits_p && chall_checkdigits)
	xmlNewProp (chall, BAD_CAST "CheckDigits", BAD_CAST "true");
    }

  if (resp_encoding_p || resp_length_p || resp_checkdigits_p)
    {
      xmlNodePtr resp;

      resp = xmlNewChild (algparm, NULL, BAD_CAST "ResponseFormat", NULL);

      if (resp_encoding_p)
	xmlNewProp (resp, BAD_CAST "Encoding",
		    BAD_CAST pskc_valueformat2str (resp_encoding));

      if (resp_length_p)
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (resp_length, buf);
	  xmlNewProp (resp, BAD_CAST "Length", BAD_CAST p);
	}

      if (resp_checkdigits_p && resp_checkdigits)
	xmlNewProp (resp, BAD_CAST "CheckDigits", BAD_CAST "true");
    }

  return PSKC_OK;
}

static int
build_data (pskc_key_t * kp, xmlNodePtr keyp)
{
  const char *b64secret = pskc_get_key_data_b64secret (kp);
  int counter_p;
  uint64_t counter = pskc_get_key_data_counter (kp, &counter_p);
  int t_p;
  uint32_t t = pskc_get_key_data_time (kp, &t_p);
  int tinterval_p;
  uint32_t tinterval = pskc_get_key_data_timeinterval (kp, &tinterval_p);
  int tdrift_p;
  uint32_t tdrift = pskc_get_key_data_timedrift (kp, &tdrift_p);
  xmlNodePtr data, sub;

  if (!b64secret && !counter_p && !t_p && !tinterval_p && !tdrift_p)
    return PSKC_OK;

  data = xmlNewChild (keyp, NULL, BAD_CAST "Data", NULL);

  if (b64secret)
    {
      sub = xmlNewChild (data, NULL, BAD_CAST "Secret", NULL);
      xmlNewTextChild (sub, NULL, BAD_CAST "PlainValue", BAD_CAST b64secret);
    }

  if (counter_p)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      char *p = umaxtostr (counter, buf);

      sub = xmlNewChild (data, NULL, BAD_CAST "Counter", NULL);
      xmlNewTextChild (sub, NULL, BAD_CAST "PlainValue", BAD_CAST p);
    }

  if (t_p)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      char *p = umaxtostr (t, buf);

      sub = xmlNewChild (data, NULL, BAD_CAST "Time", NULL);
      xmlNewTextChild (sub, NULL, BAD_CAST "PlainValue", BAD_CAST p);
    }

  if (tinterval_p)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      char *p = umaxtostr (tinterval, buf);

      sub = xmlNewChild (data, NULL, BAD_CAST "TimeInterval", NULL);
      xmlNewTextChild (sub, NULL, BAD_CAST "PlainValue", BAD_CAST p);
    }

  if (tdrift_p)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      char *p = umaxtostr (tdrift, buf);

      sub = xmlNewChild (data, NULL, BAD_CAST "TimeDrift", NULL);
      xmlNewTextChild (sub, NULL, BAD_CAST "PlainValue", BAD_CAST p);
    }

  return PSKC_OK;
}

static int
build_policy (pskc_key_t * kp, xmlNodePtr keyp)
{
  int keyusage_p;
  int keyusages = pskc_get_key_policy_keyusages (kp, &keyusage_p);
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
  int numberoftransactions_p;
  uint64_t numberoftransactions = pskc_get_key_policy_numberoftransactions
    (kp, &numberoftransactions_p);
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
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (attempts, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MaxFailedAttempts", BAD_CAST p);
	}

      if (pinmin_p)
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (pinmin, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MinLength", BAD_CAST p);
	}

      if (pinmax_p)
	{
	  char buf[INT_BUFSIZE_BOUND (uintmax_t)];
	  char *p = umaxtostr (pinmax, buf);
	  xmlNewProp (pinpolicy, BAD_CAST "MaxLength", BAD_CAST p);
	}

      if (pinencoding_p)
	xmlNewProp (pinpolicy, BAD_CAST "PINEncoding",
		    BAD_CAST pskc_valueformat2str (pinencoding));
    }

  if (keyusage_p)
    {
      int i;
      for (i = 1; i <= PSKC_KEYUSAGE_LAST; i = i << 1)
	{
	  const char *keyusage_str = pskc_keyusage2str (i);

	  if (keyusages & i)
	    xmlNewTextChild (policy, NULL, BAD_CAST "KeyUsage",
			     BAD_CAST keyusage_str);
	}
    }

  if (numberoftransactions_p)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      char *p = umaxtostr (numberoftransactions, buf);
      xmlNewTextChild (policy, NULL, BAD_CAST "NumberOfTransactions",
		       BAD_CAST p);
    }

  return PSKC_OK;
}

static int
build_key (pskc_key_t * kp, xmlNodePtr keyp)
{
  const char *id = pskc_get_key_id (kp);
  const char *alg = pskc_get_key_algorithm (kp);
  const char *issuer = pskc_get_key_issuer (kp);
  const char *userid = pskc_get_key_userid (kp);
  const char *keyprofileid = pskc_get_key_profileid (kp);
  const char *keyreference = pskc_get_key_reference (kp);
  const char *friendlyname = pskc_get_key_friendlyname (kp);
  xmlNodePtr key;
  int rc;

  key = xmlNewChild (keyp, NULL, BAD_CAST "Key", NULL);

  if (id && xmlNewProp (key, BAD_CAST "Id", BAD_CAST id) == NULL)
    return PSKC_XML_ERROR;

  if (alg && xmlNewProp (key, BAD_CAST "Algorithm", BAD_CAST alg) == NULL)
    return PSKC_XML_ERROR;

  if (issuer &&
      xmlNewTextChild (key, NULL, BAD_CAST "Issuer", BAD_CAST issuer) == NULL)
    return PSKC_XML_ERROR;

  rc = build_algparm (kp, key);
  if (rc != PSKC_OK)
    return rc;

  if (keyprofileid && xmlNewTextChild (key, NULL, BAD_CAST "KeyProfileId",
				       BAD_CAST keyprofileid) == NULL)
    return PSKC_XML_ERROR;

  if (keyreference && xmlNewTextChild (key, NULL, BAD_CAST "KeyReference",
				       BAD_CAST keyreference) == NULL)
    return PSKC_XML_ERROR;

  if (friendlyname && xmlNewTextChild (key, NULL, BAD_CAST "FriendlyName",
				       BAD_CAST friendlyname) == NULL)
    return PSKC_XML_ERROR;

  rc = build_data (kp, key);
  if (rc != PSKC_OK)
    return rc;

  if (userid && xmlNewTextChild (key, NULL, BAD_CAST "UserId",
				 BAD_CAST userid) == NULL)
    return PSKC_XML_ERROR;

  rc = build_policy (kp, key);
  if (rc != PSKC_OK)
    return rc;

  return PSKC_OK;
}

static int
build_keypackage (pskc_key_t * kp, xmlNodePtr keyp)
{
  build_deviceinfo (kp, keyp);
  build_cryptomoduleinfo (kp, keyp);
  build_key (kp, keyp);

  return PSKC_OK;
}

static int
build_keycont (pskc_t * container, xmlNodePtr keycont)
{
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
      xmlNodePtr keypackage;
      int rc;

      keypackage = xmlNewChild (keycont, NULL, BAD_CAST "KeyPackage", NULL);
      if (keypackage == NULL)
	return PSKC_XML_ERROR;
      rc = build_keypackage (kp, keypackage);
      if (rc != PSKC_OK)
	return rc;
    }

  /* KeyPackage is a required element, but can be empty. */
  if (i == 0)
    if (xmlNewChild (keycont, NULL, BAD_CAST "KeyPackage", NULL) == NULL)
      return PSKC_XML_ERROR;

  return PSKC_OK;
}

/**
 * pskc_build_xml:
 * @container: a #pskc_t handle, from pskc_init().
 * @out: pointer to output variable to hold newly allocated string.
 * @len: output variable holding length of *@out.
 *
 * This function builds a XML file from the data in @container.  As a
 * convenience, it also converts the XML into a string placed in the
 * newly allocated *@out of length @len using pskc_output() with
 * %PSKC_OUTPUT_XML.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned.
 **/
int
pskc_build_xml (pskc_t * container, char **out, size_t * len)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr keycont = NULL;
  int rc;

  doc = xmlNewDoc (BAD_CAST "1.0");
  if (doc == NULL)
    return PSKC_XML_ERROR;

  keycont = xmlNewNode (NULL, BAD_CAST "KeyContainer");
  if (keycont == NULL)
    {
      _pskc_debug ("xmlNewNode failed");
      xmlFreeDoc (doc);
      return PSKC_XML_ERROR;
    }

  rc = build_keycont (container, keycont);
  if (rc != PSKC_OK)
    {
      xmlFreeDoc (doc);
      return rc;
    }

  xmlDocSetRootElement (doc, keycont);

  if (container->xmldoc)
    xmlFreeDoc (container->xmldoc);
  container->xmldoc = doc;
  doc = NULL;

  if (out || len)
    rc = pskc_output (container, PSKC_OUTPUT_XML, out, len);
  else
    rc = PSKC_OK;

  return rc;
}
