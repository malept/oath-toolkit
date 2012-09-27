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

#include "internal.h"

#include <string.h>

static int
parse_deviceinfo (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Manufacturer", name) == 0)
	    kp->manufacturer = (char *) cur_node->children->content;
	  else if (strcmp ("SerialNo", name) == 0)
	    kp->serialno = (char *) cur_node->children->content;
	  else if (strcmp ("UserId", name) == 0)
	    kp->device_userid = (char *) cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_cryptomoduleinfo (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Id", name) == 0)
	    kp->crypto_id = (char *) cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_intlongstrdatatype (xmlNode * x, const char **var)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("PlainValue", name) == 0)
	    *var = (char *) cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_data (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
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
	    }
	  else if (strcmp ("Counter", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_counter);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("Time", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_time);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("TimeInterval", name) == 0)
	    {
	      rc = parse_intlongstrdatatype (cur_node->children,
					     &kp->key_time_interval);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_algorithmparameters (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("ResponseFormat", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Length", (const char *) cur_attr->name) == 0)
		    kp->key_alg_resp_length =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("Encoding",
				   (const char *) cur_attr->name) == 0)
		    kp->key_alg_resp_encoding =
		      (char *) cur_attr->children->content;
		  else
		    return PSKC_XML_SYNTAX_ERROR;
		}
	    }
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_policy (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void) pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("StartDate", name) == 0)
	    kp->key_policy_startdate = (char *) cur_node->children->content;
	  else if (strcmp ("ExpiryDate", name) == 0)
	    kp->key_policy_expirydate = (char *) cur_node->children->content;
	  else if (strcmp ("PINPolicy", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("MinLength",
			      (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_minlength =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("MaxLength",
				   (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_maxlength =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("PINKeyId",
				   (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_pinkeyid =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("PINEncoding",
				   (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_pinencoding =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("PINUsageMode",
				   (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_pinusagemode =
		      (char *) cur_attr->children->content;
		  else if (strcmp ("MaxFailedAttempts",
				   (const char *) cur_attr->name) == 0)
		    kp->key_pinpolicy_maxfailedattempts =
		      (char *) cur_attr->children->content;
		  else
		    return PSKC_XML_SYNTAX_ERROR;
		}
	    }
	  else if (strcmp ("KeyUsage", name) == 0)
	    {
	      kp->key_usage = (char *) cur_node->children->content;
	    }
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_key (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  int rc;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("AlgorithmParameters", name) == 0)
	    {
	      rc = parse_algorithmparameters (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("Data", name) == 0)
	    {
	      rc = parse_data (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("Policy", name) == 0)
	    {
	      rc = parse_policy (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else if (strcmp ("Issuer", name) == 0)
	    kp->key_issuer = (char *) cur_node->children->content;
	  else if (strcmp ("KeyProfileId", name) == 0)
	    kp->key_profileid = (char *) cur_node->children->content;
	  else if (strcmp ("KeyReference", name) == 0)
	    kp->key_reference = (char *) cur_node->children->content;
	  else if (strcmp ("UserId", name) == 0)
	    kp->key_userid = (char *) cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_keypackage (pskc * pd, xmlNode * x, struct pskc_keypackage *kp)
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
		  else
		    return PSKC_XML_SYNTAX_ERROR;
		}

	      rc = parse_key (pd, cur_node->children, kp);
	      if (rc != PSKC_OK)
		return rc;
	    }
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
    }

  return PSKC_OK;
}

static int
parse_keypackages (pskc * pd, xmlNode * x)
{
  xmlNode *cur_node = NULL;
  int rc;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("KeyPackage", name) != 0)
	    return PSKC_XML_SYNTAX_ERROR;
	  else if (cur_node->properties != NULL)
	    return PSKC_XML_SYNTAX_ERROR;
	  else if (cur_node->children == NULL)
	    return PSKC_XML_SYNTAX_ERROR;
	  else
	    {
	      struct pskc_keypackage *tmp;

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
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_keycontainer (pskc * pd, xmlNode * x)
{
  xmlAttr *cur_attr = NULL;
  const char *name = (const char *) x->name;

  if (strcmp ("KeyContainer", name) != 0)
    return PSKC_XML_SYNTAX_ERROR;

  if (x->next != NULL)
    return PSKC_XML_SYNTAX_ERROR;

  if (x->properties == NULL)
    return PSKC_XML_SYNTAX_ERROR;

  for (cur_attr = x->properties; cur_attr; cur_attr = cur_attr->next)
    {
      if (strcmp ("Version", (const char *) cur_attr->name) == 0)
	pd->version = (char *) cur_attr->children->content;
      else if (strcmp ("Id", (const char *) cur_attr->name) == 0)
	pd->id = (char *) cur_attr->children->content;
      else
	return PSKC_XML_SYNTAX_ERROR;
    }

  if (pd->version == NULL)
    return PSKC_XML_SYNTAX_ERROR;

  return parse_keypackages (pd, x->children);
}

int
_pskc_parse (pskc * container)
{
  xmlNode *root = NULL;
  int rc;

  root = xmlDocGetRootElement (container->xmldoc);

  rc = parse_keycontainer (container, root);
  if (rc != PSKC_OK)
    return rc;

  return PSKC_OK;
}
