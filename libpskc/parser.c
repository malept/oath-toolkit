/*
 * parser.c - parse PSKC data structure in XML and convert to internal format.
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

#include <string.h>

static int
parse_deviceinfo (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Manufacturer", name) == 0)
	    kp->device_manufacturer = (char *) cur_node->children->content;
	  else if (strcmp ("SerialNo", name) == 0)
	    kp->device_serialno = (char *) cur_node->children->content;
	  else if (strcmp ("Model", name) == 0)
	    kp->device_model = (char *) cur_node->children->content;
	  else if (strcmp ("Model", name) == 0)
	    kp->device_model = (char *) cur_node->children->content;
	  else if (strcmp ("IssueNo", name) == 0)
	    kp->device_issueno = (char *) cur_node->children->content;
	  else if (strcmp ("DeviceBinding", name) == 0)
	    kp->device_devicebinding = (char *) cur_node->children->content;
	  else if (strcmp ("StartDate", name) == 0)
	    {
	      kp->device_startdate_str =
		(char *) cur_node->children->content;
	      memset (&kp->device_startdate, 0, sizeof(struct tm));
	      strptime (kp->device_startdate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->device_startdate);
	    }
	  else if (strcmp ("ExpiryDate", name) == 0)
	    {
	      kp->device_expirydate_str =
		(char *) cur_node->children->content;
	      memset (&kp->device_expirydate, 0, sizeof(struct tm));
	      strptime (kp->device_expirydate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->device_expirydate);
	    }
	  else if (strcmp ("UserId", name) == 0)
	    kp->device_userid = (char *) cur_node->children->content;
	}
    }

  return PSKC_OK;
}

static int
parse_cryptomoduleinfo (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Id", name) == 0)
	    kp->cryptomodule_id = (char *) cur_node->children->content;
	}
    }

  return PSKC_OK;
}

static int
parse_intlongstrdatatype (xmlNode * x, const char **var)
{
  xmlNode *cur_node = NULL;

  *var = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("PlainValue", name) == 0)
	    *var = (char *) cur_node->children->content;
	}
    }

  return PSKC_OK;
}

static int
parse_data (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  int rc;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Secret", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_secret);
	      if (rc != PSKC_OK)
		return rc;
	      /* b64 decode */
	      if (kp->key_secret)
		kp->key_secret_len = strlen (kp->key_secret);
	    }
	  else if (strcmp ("Counter", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_counter_str);
	      if (rc != PSKC_OK)
		return rc;
	      if (kp->key_counter_str)
		kp->key_counter = strtoull (kp->key_counter_str, NULL, 10);
	    }
	  else if (strcmp ("Time", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_time_str);
	      if (rc != PSKC_OK)
		return rc;
	      if (kp->key_time_str)
		kp->key_time = strtoul (kp->key_time_str, NULL, 10);
	    }
	  else if (strcmp ("TimeInterval", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_timeinterval_str);
	      if (rc != PSKC_OK)
		return rc;
	      if (kp->key_timeinterval_str)
		kp->key_timeinterval = strtoul (kp->key_timeinterval_str,
						NULL, 10);
	    }
	  else if (strcmp ("TimeDrift", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_timedrift_str);
	      if (rc != PSKC_OK)
		return rc;
	      if (kp->key_timedrift_str)
		kp->key_timedrift = strtoul (kp->key_timedrift_str, NULL, 10);
	    }
	}
    }

  return PSKC_OK;
}

static int
parse_algorithmparameters (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Suite", name) == 0)
	    kp->key_algparm_suite = (char *) cur_node->children->content;
	  else if (strcmp ("ResponseFormat", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Encoding",
			      (const char *) cur_attr->name) == 0)
		    {
		      kp->key_algparm_resp_encoding_str =
			(char *) cur_attr->children->content;
		      kp->key_algparm_resp_encoding =
			pskc_str2valueformat
			(kp->key_algparm_resp_encoding_str);
		    }
		  else if (strcmp ("Length", (const char *) cur_attr->name) == 0)
		    {
		      kp->key_algparm_resp_length_str =
			(char *) cur_attr->children->content;
		      kp->key_algparm_resp_length =
			strtoul (kp->key_algparm_resp_length_str, NULL, 10);
		    }
		  else if (strcmp ("CheckDigits", (const char *) cur_attr->name) == 0)
		    {
		      kp->key_algparm_resp_checkdigits_str =
			(char *) cur_attr->children->content;
		      if (strcmp ("1", kp->
				  key_algparm_resp_checkdigits_str) == 0)
			kp->key_algparm_resp_checkdigits = 1;
		      else if (strcmp ("true", kp->
				       key_algparm_resp_checkdigits_str) == 0)
			kp->key_algparm_resp_checkdigits = 1;
		      else
			kp->key_algparm_resp_checkdigits = 0;
		    }
		}
	    }
	  /* XXX incomplete */
	}
    }

  return PSKC_OK;
}

static int
parse_policy (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("StartDate", name) == 0)
	    {
	      kp->key_policy_startdate_str =
		(char *) cur_node->children->content;
	      memset (&kp->key_policy_startdate, 0, sizeof(struct tm));
	      strptime (kp->key_policy_startdate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->key_policy_startdate);
	    }
	  else if (strcmp ("ExpiryDate", name) == 0)
	    {
	      kp->key_policy_expirydate_str =
		(char *) cur_node->children->content;
	      memset (&kp->key_policy_expirydate, 0, sizeof(struct tm));
	      strptime (kp->key_policy_expirydate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->key_policy_expirydate);
	    }
	  else if (strcmp ("PINPolicy", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("PINKeyId",
			      (const char *) cur_attr->name) == 0)
		    kp->key_policy_pinkeyid =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("PINUsageMode",
				   (const char *) cur_attr->name) == 0)
		    {
		      kp->key_policy_pinusagemode_str =
			(char *) cur_attr->children->content;
		      kp->key_policy_pinusagemode =
			pskc_str2pinusagemode (kp->
					       key_policy_pinusagemode_str);
		    }
		  else if (strcmp ("MaxFailedAttempts",
				   (const char *) cur_attr->name) == 0)
		    {
		      kp->key_policy_pinmaxfailedattempts_str =
			(char *) cur_attr->children->content;
		      kp->key_policy_pinmaxfailedattempts =
			strtoull (kp->key_policy_pinmaxfailedattempts_str,
				  NULL, 10);
		    }
		  else if (strcmp ("MinLength",
			      (const char *) cur_attr->name) == 0)
		    {
		      kp->key_policy_pinminlength_str =
			(char *) cur_attr->children->content;
		      kp->key_policy_pinminlength =
			strtoull (kp->key_policy_pinminlength_str, NULL, 10);
		    }
		  else if (strcmp ("MaxLength",
				   (const char *) cur_attr->name) == 0)
		    {
		      kp->key_policy_pinmaxlength_str =
			(char *) cur_attr->children->content;
		      kp->key_policy_pinmaxlength =
			strtoull (kp->key_policy_pinmaxlength_str, NULL, 10);
		    }
		  else if (strcmp ("PINEncoding",
				   (const char *) cur_attr->name) == 0)
		    {
		      kp->key_policy_pinencoding_str =
			(char *) cur_attr->children->content;
		      kp->key_policy_pinencoding =
			pskc_str2valueformat (kp->key_policy_pinencoding_str);
		    }
	    }
	    }
	  else if (strcmp ("KeyUsage", name) == 0)
	    {
	      kp->key_policy_keyusage_str =
		(char *) cur_node->children->content;
	      kp->key_policy_keyusage |=
		pskc_str2keyusage (kp->key_policy_keyusage_str);
	    }
	  else if (strcmp ("NumberOfTransactions", name) == 0)
	    {
	      int rc = parse_intlongstrdatatype
		(cur_node->children, &kp->key_policy_numberoftransactions_str);
	      if (rc != PSKC_OK)
		return rc;
	      kp->key_policy_numberoftransactions =
		strtoull (kp->key_policy_numberoftransactions_str, NULL, 10);
	    }
	}
    }

  return PSKC_OK;
}

static int
parse_key (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  int rc;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Issuer", name) == 0)
	    kp->key_issuer = (char *) cur_node->children->content;
	  else if (strcmp ("AlgorithmParameters", name) == 0)
	    {
	      rc = parse_algorithmparameters (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("KeyProfileId", name) == 0)
	    kp->key_profileid = (char *) cur_node->children->content;
	  else if (strcmp ("KeyReference", name) == 0)
	    kp->key_reference = (char *) cur_node->children->content;
	  else if (strcmp ("FriendlyName", name) == 0)
	    kp->key_friendlyname = (char *) cur_node->children->content;
	  else if (strcmp ("Data", name) == 0)
	    {
	      rc = parse_data (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("UserId", name) == 0)
	    kp->key_userid = (char *) cur_node->children->content;
	  else if (strcmp ("Policy", name) == 0)
	    {
	      rc = parse_policy (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	}
    }

  return PSKC_OK;
}

static int
parse_keypackage (pskc_t * pd, xmlNode * x, struct pskc_key *kp)
{
  xmlNode *cur_node = NULL;
  int rc;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("DeviceInfo", name) == 0)
	    {
	      rc = parse_deviceinfo (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("CryptoModuleInfo", name) == 0)
	    {
	      rc = parse_cryptomoduleinfo (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("Key", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Id", (const char *) cur_attr->name) == 0)
		    kp->key_id = (char *) cur_attr->children->content;
		  else if (strcmp ("Algorithm",
				   (const char *) cur_attr->name) == 0)
		    kp->key_algorithm = (char *) cur_attr->children->content;
		}

	      rc = parse_key (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	}
    }

  return PSKC_OK;
}

static int
parse_keypackages (pskc_t * pd, xmlNode * x)
{
  xmlNode *cur_node = NULL;
  int rc;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("KeyPackage", name) == 0)
	    {
	      struct pskc_key *tmp;

	      tmp = realloc (pd->keypackages,
			     sizeof (*pd->keypackages) *
			     (pd->nkeypackages + 1));
	      if (tmp == NULL)
		return PSKC_MALLOC_ERROR;
	      pd->keypackages = tmp;
	      pd->nkeypackages++;

	      memset (&pd->keypackages[pd->nkeypackages - 1], 0,
		      sizeof (*pd->keypackages));

	      rc = parse_keypackage (pd, cur_node->children,
				     &pd->keypackages[pd->nkeypackages - 1]);
	      if (rc != PSKC_OK)
		return rc;
	    }
	}
    }

  return PSKC_OK;
}

static int
parse_keycontainer (pskc_t * pd, xmlNode * x)
{
  xmlAttr *cur_attr = NULL;
  const char *name = (const char *) x->name;

  if (strcmp ("KeyContainer", name) == 0)
    for (cur_attr = x->properties; cur_attr; cur_attr = cur_attr->next)
      {
	if (strcmp ("Version", (const char *) cur_attr->name) == 0)
	  pd->version = (char *) cur_attr->children->content;
	else if (strcmp ("Id", (const char *) cur_attr->name) == 0)
	  pd->id = (char *) cur_attr->children->content;
      }

  return parse_keypackages (pd, x->children);
}

int
_pskc_parse (pskc_t * container)
{
  xmlNode *root = NULL;
  int rc;

  root = xmlDocGetRootElement (container->xmldoc);

  rc = parse_keycontainer (container, root);
  if (rc != PSKC_OK)
    return rc;

  return PSKC_OK;
}
