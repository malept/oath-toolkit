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
parse_deviceinfo (pskc_data *pd, xmlNode *x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void)pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Manufacturer", name) == 0)
	    kp->manufacturer = cur_node->children->content;
	  else if (strcmp ("SerialNo", name) == 0)
	    kp->serialno = cur_node->children->content;
	  else if (strcmp ("UserId", name) == 0)
	    kp->device_userid = cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_intlongstrdatatype (xmlNode *x, char **var)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("PlainValue", name) == 0)
	    *var = cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_data (pskc_data *pd, xmlNode *x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  int rc;

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
parse_algorithmparameters (pskc_data *pd, xmlNode *x,
			   struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void)pd;

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
		    kp->key_alg_resp_length = cur_attr->children->content;
		  else if (strcmp ("Encoding",
				   (const char *) cur_attr->name) == 0)
		    kp->key_alg_resp_encoding = cur_attr->children->content;
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
parse_policy (pskc_data *pd, xmlNode *x, struct pskc_keypackage *kp)
{
  xmlNode *cur_node = NULL;
  (void)pd;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("StartDate", name) == 0)
	    kp->key_policy_startdate = cur_node->children->content;
	  else if (strcmp ("ExpiryDate", name) == 0)
	    kp->key_policy_expirydate = cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_key (pskc_data *pd, xmlNode *x, struct pskc_keypackage *kp)
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
	    kp->key_issuer = cur_node->children->content;
	  else if (strcmp ("UserId", name) == 0)
	    kp->key_userid = cur_node->children->content;
	  else
	    return PSKC_XML_SYNTAX_ERROR;
	}
      else if (cur_node->type != XML_TEXT_NODE)
	return PSKC_XML_SYNTAX_ERROR;
    }

  return PSKC_OK;
}

static int
parse_keypackage (pskc_data *pd, xmlNode *x, struct pskc_keypackage *kp)
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
	  else if (strcmp ("Key", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Id", (const char *) cur_attr->name) == 0)
		    kp->key_id = cur_attr->children->content;
		  else if (strcmp ("Algorithm",
				   (const char *) cur_attr->name) == 0)
		    kp->key_algorithm = cur_attr->children->content;
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
parse_keypackages (pskc_data *pd, xmlNode *x)
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
parse_keycontainer (pskc_data *pd, xmlNode *x)
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
	pd->version = cur_attr->children->content;
      else if (strcmp ("Id", (const char *) cur_attr->name) == 0)
	pd->id = cur_attr->children->content;
      else
	return PSKC_XML_SYNTAX_ERROR;
    }

  if (pd->version == NULL)
    return PSKC_XML_SYNTAX_ERROR;

  return parse_keypackages (pd, x->children);
}

int
_pskc_parse (pskc_data *pd)
{
  xmlNode *root_element = NULL;
  int rc;

  root_element = xmlDocGetRootElement(pd->xmldoc);

  rc = parse_keycontainer(pd, root_element);
  if (rc != PSKC_OK)
    return rc;

  return PSKC_OK;
}
