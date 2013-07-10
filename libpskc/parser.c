/*
 * parser.c - Parse PSKC data structure in XML and convert to internal format.
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
#define INTERNAL_NEED_PSKC_KEY_STRUCT
#include "internal.h"

#include <string.h>
#include "base64.h"

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
	{
	  kp->device_manufacturer = content;
	  if (strncmp ("oath.", content, 5) != 0
	      && strncmp ("iana.", content, 5) != 0)
	    _pskc_debug ("non-compliant Manufacturer value: %s", content);
	}
      else if (strcmp ("SerialNo", name) == 0)
	kp->device_serialno = content;
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
	  memset (&kp->device_startdate, 0, sizeof (struct tm));
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
	  memset (&kp->device_expirydate, 0, sizeof (struct tm));
	  p = strptime (kp->device_expirydate_str,
			"%Y-%m-%dT%H:%M:%SZ", &kp->device_expirydate);
	  if (p == NULL || *p != '\0')
	    {
	      _pskc_debug ("cannot convert time string '%s'",
			   kp->device_expirydate_str);
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

static char *
remove_whitespace (const char *str, size_t * outlen)
{
  size_t len = strlen (str);
  char *out = malloc (len + 1);
  size_t i, j;

  if (out == NULL)
    return NULL;

  for (i = 0, j = 0; i < len; i++)
    if (isbase64 (str[i]) || str[i] == '=')
      out[j++] = str[i];

  out[j] = '\0';

  *outlen = j;

  return out;
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
				    &kp->key_secret_str, rc);
	  if (kp->key_secret_str)
	    {
	      bool ok;
	      size_t l;

	      kp->key_b64secret = remove_whitespace (kp->key_secret_str, &l);
	      if (kp->key_b64secret == NULL)
		{
		  _pskc_debug ("base64 whitespace malloc failed");
		  *rc = PSKC_MALLOC_ERROR;
		}
	      else
		{
		  ok = base64_decode_alloc (kp->key_b64secret, l,
					    &kp->key_secret,
					    &kp->key_secret_len);
		  if (!ok)
		    {
		      _pskc_debug ("base64 decoding failed");
		      *rc = PSKC_BASE64_ERROR;
		    }
		  if (kp->key_secret == NULL)
		    {
		      _pskc_debug ("base64 malloc failed");
		      *rc = PSKC_MALLOC_ERROR;
		    }
		}
	    }
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
      else if (strcmp ("ChallengeFormat", name) == 0)
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
		  kp->key_algparm_chall_encoding_str = attr_content;
		  kp->key_algparm_chall_encoding =
		    pskc_str2valueformat (kp->key_algparm_chall_encoding_str);
		}
	      else if (strcmp ("Min", attr_name) == 0)
		{
		  kp->key_algparm_chall_min_str = attr_content;
		  kp->key_algparm_chall_min =
		    strtoul (kp->key_algparm_chall_min_str, NULL, 10);
		}
	      else if (strcmp ("Max", attr_name) == 0)
		{
		  kp->key_algparm_chall_max_str = attr_content;
		  kp->key_algparm_chall_max =
		    strtoul (kp->key_algparm_chall_max_str, NULL, 10);
		}
	      else if (strcmp ("CheckDigits", attr_name) == 0)
		{
		  kp->key_algparm_chall_checkdigits_str = attr_content;
		  if (strcmp ("1", kp->key_algparm_chall_checkdigits_str) ==
		      0)
		    kp->key_algparm_chall_checkdigits = 1;
		  else if (strcmp ("true",
				   kp->key_algparm_chall_checkdigits_str) ==
			   0)
		    kp->key_algparm_chall_checkdigits = 1;
		  else
		    kp->key_algparm_chall_checkdigits = 0;
		}
	      else
		{
		  _pskc_debug ("unknown <%s> attribute <%s>",
			       name, attr_name);
		  *rc = PSKC_PARSE_ERROR;
		}
	    }
	}
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
		    pskc_str2valueformat (kp->key_algparm_resp_encoding_str);
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
		  if (strcmp ("1", kp->key_algparm_resp_checkdigits_str) == 0)
		    kp->key_algparm_resp_checkdigits = 1;
		  else if (strcmp ("true",
				   kp->key_algparm_resp_checkdigits_str) == 0)
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
	  memset (&kp->key_policy_startdate, 0, sizeof (struct tm));
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
	  memset (&kp->key_policy_expirydate, 0, sizeof (struct tm));
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
		    pskc_str2pinusagemode (kp->key_policy_pinusagemode_str);
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
		  _pskc_debug ("unknown <%s> attribute <%s>", name,
			       attr_name);
		  *rc = PSKC_PARSE_ERROR;
		}
	    }
	}
      else if (strcmp ("KeyUsage", name) == 0)
	{
	  kp->key_policy_keyusage_str = content;
	  kp->key_policy_keyusages |=
	    pskc_str2keyusage (kp->key_policy_keyusage_str);
	}
      else if (strcmp ("NumberOfTransactions", name) == 0)
	{
	  kp->key_policy_numberoftransactions_str = content;
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
		  _pskc_debug ("unknown <%s> attribute <%s>", name,
			       attr_name);
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
			 sizeof (*pd->keypackages) * (pd->nkeypackages + 1));
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
      else if (strcmp ("Signature", name) == 0)
	pd->signed_p = 1;
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

/**
 * pskc_init:
 * @container: pointer to a #pskc_t handle to initialize.
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
 * pskc_done:
 * @container: a #pskc_t handle, from pskc_init().
 *
 * This function releases the resources associated with the PSKC
 * @container handle.
 **/
void
pskc_done (pskc_t * container)
{
  size_t i;

  if (container == NULL)
    return;

  xmlFreeDoc (container->xmldoc);

  for (i = 0; i < container->nkeypackages; i++)
    {
      pskc_key_t *kp = &container->keypackages[i];
      free (kp->key_secret);
      free (kp->key_b64secret);
    }

  free (container->keypackages);
  free (container);
}

/**
 * pskc_parse_from_memory:
 * @container: a #pskc_t handle, from pskc_init().
 * @len: length of @buffer.
 * @buffer: XML data to parse.
 *
 * This function will parse the XML data in @buffer of @len size into
 * @container.  If %PSKC_PARSE_ERROR is returned, parsing of some
 * elements have failed but the @container is still valid and contain
 * partially parsed information.  In this situation, you may continue
 * but raise a warning.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned, on XML library
 *   errors %PSKC_XML_ERROR is returned, on PSKC parse errors
 *   %PSKC_PARSE_ERROR is returned.
 **/
int
pskc_parse_from_memory (pskc_t * container, size_t len, const char *buffer)
{
  xmlDocPtr xmldoc;
  xmlNode *root;
  int rc = PSKC_OK;

  xmldoc = xmlReadMemory (buffer, len, NULL, NULL, XML_PARSE_NONET);
  if (xmldoc == NULL)
    return PSKC_XML_ERROR;

  container->xmldoc = xmldoc;

  root = xmlDocGetRootElement (xmldoc);
  parse_keycontainer (container, root, &rc);

  return rc;
}
