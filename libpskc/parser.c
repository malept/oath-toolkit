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

static void
parse_deviceinfo (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("Manufacturer", name) == 0)
	kp->device_manufacturer = content;
      else if (strcmp ("SerialNo", name) == 0)
	kp->device_serialno = content;
      else if (strcmp ("Model", name) == 0)
	kp->device_model = content;
      else if (strcmp ("Model", name) == 0)
	kp->device_model = content;
      else if (strcmp ("IssueNo", name) == 0)
	kp->device_issueno = content;
      else if (strcmp ("DeviceBinding", name) == 0)
	kp->device_devicebinding = content;
      else if (strcmp ("StartDate", name) == 0)
	{
	  const char *p;
	  kp->device_startdate_str = content;
	  memset (&kp->device_startdate, 0, sizeof(struct tm));
	  p = strptime (kp->device_startdate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->device_startdate);
	  if (p == NULL || *p != '\0')
	    {
	      _pskc_debug ("cannot convert time string '%s'",
			   kp->device_startdate_str);
	      *rc = PSKC_PARSE_ERROR;
	    }
	}
      else if (strcmp ("ExpiryDate", name) == 0)
	{
	  const char *p;
	  kp->device_expirydate_str = content;
	  memset (&kp->device_expirydate, 0, sizeof(struct tm));
	  p = strptime (kp->device_expirydate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->device_expirydate);
	  if (p == NULL || *p != '\0')
	    {
	      _pskc_debug ("cannot convert time string '%s'",
			   kp->device_startdate_str);
	      *rc = PSKC_PARSE_ERROR;
	    }
	}
      else if (strcmp ("UserId", name) == 0)
	kp->device_userid = content;
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_cryptomoduleinfo (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("Id", name) == 0)
	kp->cryptomodule_id = content;
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_intlongstrdatatype (xmlNode * x, const char **var, int *rc)
{
  xmlNode *cur_node = NULL;

  *var = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("PlainValue", name) == 0)
	*var = content;
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_data (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("Secret", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_secret, rc);
	  /* b64 decode */
	  if (kp->key_secret)
	    kp->key_secret_len = strlen (kp->key_secret);
	}
      else if (strcmp ("Counter", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_counter_str, rc);
	  if (kp->key_counter_str)
	    kp->key_counter = strtoull (kp->key_counter_str, NULL, 10);
	}
      else if (strcmp ("Time", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_time_str, rc);
	  if (kp->key_time_str)
	    kp->key_time = strtoul (kp->key_time_str, NULL, 10);
	}
      else if (strcmp ("TimeInterval", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_timeinterval_str, rc);
	  if (kp->key_timeinterval_str)
	    kp->key_timeinterval = strtoul (kp->key_timeinterval_str,
					    NULL, 10);
	}
      else if (strcmp ("TimeDrift", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_timedrift_str, rc);
	  if (kp->key_timedrift_str)
	    kp->key_timedrift = strtoul (kp->key_timedrift_str, NULL, 10);
	}
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_algorithmparameters (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("Suite", name) == 0)
	kp->key_algparm_suite = content;
      else if (strcmp ("ResponseFormat", name) == 0)
	{
	  xmlAttr *cur_attr = NULL;

	  for (cur_attr = cur_node->properties; cur_attr;
	       cur_attr = cur_attr->next)
	    {
	      const char *attr_name = (const char *) cur_attr->name;
	      const char *attr_content =
		(const char *) cur_attr->children->content;

	      if (strcmp ("Encoding", attr_name) == 0)
		{
		  kp->key_algparm_resp_encoding_str = attr_content;
		  kp->key_algparm_resp_encoding =
		    pskc_str2valueformat
		    (kp->key_algparm_resp_encoding_str);
		}
	      else if (strcmp ("Length", attr_name) == 0)
		{
		  kp->key_algparm_resp_length_str = attr_content;
		  kp->key_algparm_resp_length =
		    strtoul (kp->key_algparm_resp_length_str, NULL, 10);
		}
	      else if (strcmp ("CheckDigits", attr_name) == 0)
		{
		  kp->key_algparm_resp_checkdigits_str = attr_content;
		  if (strcmp ("1", kp->
			      key_algparm_resp_checkdigits_str) == 0)
		    kp->key_algparm_resp_checkdigits = 1;
		  else if (strcmp ("true", kp->
				   key_algparm_resp_checkdigits_str) == 0)
		    kp->key_algparm_resp_checkdigits = 1;
		  else
		    kp->key_algparm_resp_checkdigits = 0;
		}
	      else
		{
		  _pskc_debug ("unknown <%s> attribute <%s>",
			       name, attr_name);
		  *rc = PSKC_PARSE_ERROR;
		}
	    }
	}
      /* XXX incomplete */
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_policy (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("StartDate", name) == 0)
	{
	  const char *p;
	  kp->key_policy_startdate_str = content;
	  memset (&kp->key_policy_startdate, 0, sizeof(struct tm));
	  p = strptime (kp->key_policy_startdate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->key_policy_startdate);
	  if (p == NULL || *p != '\0')
	    {
	      _pskc_debug ("cannot convert time string '%s'",
			   kp->key_policy_startdate_str);
	      *rc = PSKC_PARSE_ERROR;
	    }
	}
      else if (strcmp ("ExpiryDate", name) == 0)
	{
	  const char *p;
	  kp->key_policy_expirydate_str = content;
	  memset (&kp->key_policy_expirydate, 0, sizeof(struct tm));
	  p = strptime (kp->key_policy_expirydate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->key_policy_expirydate);
	  if (p == NULL || *p != '\0')
	    {
	      _pskc_debug ("cannot convert time string '%s'",
			   kp->key_policy_startdate_str);
	      *rc = PSKC_PARSE_ERROR;
	    }
	}
      else if (strcmp ("PINPolicy", name) == 0)
	{
	  xmlAttr *cur_attr = NULL;

	  for (cur_attr = cur_node->properties; cur_attr;
	       cur_attr = cur_attr->next)
	    {
	      const char *attr_name = (const char *) cur_attr->name;
	      const char *attr_content =
		(const char *) cur_attr->children->content;

	      if (strcmp ("PINKeyId", attr_name) == 0)
		kp->key_policy_pinkeyid = attr_content;
	      else if (strcmp ("PINUsageMode", attr_name) == 0)
		{
		  kp->key_policy_pinusagemode_str = attr_content;
		  kp->key_policy_pinusagemode =
		    pskc_str2pinusagemode (kp->
					   key_policy_pinusagemode_str);
		}
	      else if (strcmp ("MaxFailedAttempts", attr_name) == 0)
		{
		  kp->key_policy_pinmaxfailedattempts_str = attr_content;
		  kp->key_policy_pinmaxfailedattempts =
		    strtoull (kp->key_policy_pinmaxfailedattempts_str,
			      NULL, 10);
		}
	      else if (strcmp ("MinLength", attr_name) == 0)
		{
		  kp->key_policy_pinminlength_str = attr_content;
		  kp->key_policy_pinminlength =
		    strtoull (kp->key_policy_pinminlength_str, NULL, 10);
		}
	      else if (strcmp ("MaxLength", attr_name) == 0)
		{
		  kp->key_policy_pinmaxlength_str = attr_content;
		  kp->key_policy_pinmaxlength =
			strtoull (kp->key_policy_pinmaxlength_str, NULL, 10);
		}
	      else if (strcmp ("PINEncoding", attr_name) == 0)
		{
		  kp->key_policy_pinencoding_str = attr_content;
		  kp->key_policy_pinencoding =
		    pskc_str2valueformat (kp->key_policy_pinencoding_str);
		}
	      else
		{
		  _pskc_debug ("unknown <%s> attribute <%s>", name, attr_name);
		  *rc = PSKC_PARSE_ERROR;
		}
	    }
	}
      else if (strcmp ("KeyUsage", name) == 0)
	{
	  kp->key_policy_keyusage_str = content;
	  kp->key_policy_keyusage |=
	    pskc_str2keyusage (kp->key_policy_keyusage_str);
	}
      else if (strcmp ("NumberOfTransactions", name) == 0)
	{
	  parse_intlongstrdatatype (cur_node->children,
				    &kp->key_policy_numberoftransactions_str,
				    rc);
	  if (kp->key_policy_numberoftransactions_str)
	    kp->key_policy_numberoftransactions =
	      strtoull (kp->key_policy_numberoftransactions_str, NULL, 10);
	}
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_key (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;
      const char *content = (const char *)
	(cur_node->children ? cur_node->children->content : NULL);

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("Issuer", name) == 0)
	kp->key_issuer = content;
      else if (strcmp ("AlgorithmParameters", name) == 0)
	parse_algorithmparameters (cur_node->children, kp, rc);
      else if (strcmp ("KeyProfileId", name) == 0)
	kp->key_profileid = content;
      else if (strcmp ("KeyReference", name) == 0)
	kp->key_reference = content;
      else if (strcmp ("FriendlyName", name) == 0)
	kp->key_friendlyname = content;
      else if (strcmp ("Data", name) == 0)
	parse_data (cur_node->children, kp, rc);
      else if (strcmp ("UserId", name) == 0)
	kp->key_userid = content;
      else if (strcmp ("Policy", name) == 0)
	parse_policy (cur_node->children, kp, rc);
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_keypackage (xmlNode * x, struct pskc_key *kp, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("DeviceInfo", name) == 0)
	parse_deviceinfo (cur_node->children, kp, rc);
      else if (strcmp ("CryptoModuleInfo", name) == 0)
	parse_cryptomoduleinfo (cur_node->children, kp, rc);
      else if (strcmp ("Key", name) == 0)
	{
	  xmlAttr *cur_attr = NULL;

	  for (cur_attr = cur_node->properties; cur_attr;
	       cur_attr = cur_attr->next)
	    {
	      const char *attr_name = (const char *) cur_attr->name;
	      const char *attr_content =
		(const char *) cur_attr->children->content;

	      if (strcmp ("Id", attr_name) == 0)
		kp->key_id = attr_content;
	      else if (strcmp ("Algorithm", attr_name) == 0)
		kp->key_algorithm = attr_content;
	      else
		{
		  _pskc_debug ("unknown <%s> attribute <%s>", name, attr_name);
		  *rc = PSKC_PARSE_ERROR;
		}
	    }

	  parse_key (cur_node->children, kp, rc);
	}
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_keypackages (pskc_t * pd, xmlNode * x, int *rc)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      const char *name = (const char *) cur_node->name;

      if (cur_node->type != XML_ELEMENT_NODE)
	continue;

      if (strcmp ("KeyPackage", name) == 0)
	{
	  struct pskc_key *tmp;

	  tmp = realloc (pd->keypackages,
			 sizeof (*pd->keypackages) *
			 (pd->nkeypackages + 1));
	  if (tmp == NULL)
	    {
	      *rc = PSKC_MALLOC_ERROR;
	      return;
	    }

	  pd->keypackages = tmp;
	  pd->nkeypackages++;

	  memset (&pd->keypackages[pd->nkeypackages - 1], 0,
		  sizeof (*pd->keypackages));

	  parse_keypackage (cur_node->children,
			    &pd->keypackages[pd->nkeypackages - 1], rc);
	}
      else
	{
	  _pskc_debug ("unknown <%s> element <%s>", x->parent->name, name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }
}

static void
parse_keycontainer (pskc_t * pd, xmlNode * x, int *rc)
{
  xmlAttr *cur_attr = NULL;
  const char *name = (const char *) x->name;

  if (strcmp ("KeyContainer", name) != 0)
    {
      _pskc_debug ("unknown top-level element <%s>", name);
      *rc = PSKC_PARSE_ERROR;
    }

  for (cur_attr = x->properties; cur_attr; cur_attr = cur_attr->next)
    {
      const char *attr_name = (const char *) cur_attr->name;
      const char *attr_content = (const char *) cur_attr->children->content;

      if (strcmp ("Version", attr_name) == 0)
	pd->version = attr_content;
      else if (strcmp ("Id", attr_name) == 0)
	pd->id = attr_content;
      else
	{
	  _pskc_debug ("unknown <%s> attribute <%s>", name, attr_name);
	  *rc = PSKC_PARSE_ERROR;
	}
    }

  parse_keypackages (pd, x->children, rc);
}

int
_pskc_parse (pskc_t * container)
{
  xmlNode *root = NULL;
  int rc = PSKC_OK;

  root = xmlDocGetRootElement (container->xmldoc);

  parse_keycontainer (container, root, &rc);

  return rc;
}
