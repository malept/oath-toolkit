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
print_deviceinfo (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Manufacturer", name) == 0)
	    buffer_addf (buf, "\t\t\tManufacturer: %s\n",
			 cur_node->children->content);
	  else if (strcmp ("SerialNo", name) == 0)
	    buffer_addf (buf, "\t\t\tSerialNo: %s\n",
			 cur_node->children->content);
	  else
	    buffer_addf (buf, "warning: unsupported DeviceInfo member '%s'\n",
			 name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_intlongstrdatatype (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("PlainValue", name) == 0)
	    buffer_addf (buf, "\t\t\t\t\tPlainValue: %s\n",
			 cur_node->children->content);
	  else
	    buffer_addf (buf, "warning: unsupported intDataType member '%s'\n",
			 name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_algorithmparameters (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("ResponseFormat", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;
	      buffer_addz (buf, "\t\t\t\tResponseFormat:\n");

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Length", (const char *) cur_attr->name) == 0)
		    buffer_addf (buf, "\t\t\t\t\tLength: %s\n",
				 (const char *) cur_attr->children->content);
		  else if (strcmp ("Encoding",
				   (const char *) cur_attr->name) == 0)
		    buffer_addf (buf, "\t\t\t\t\tEncoding: %s\n",
				 (const char *) cur_attr->children->content);
		  else
		    buffer_addf (buf, "warning: unknown attribute: %s\n",
				 (const char *) cur_attr->name);
		}
	    }
	  else
	    buffer_addf (buf, "warning: unsupported "
			 "AlgorithmParameters member '%s'\n", name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_data (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("Secret", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\t\tSecret:\n");
	      print_intlongstrdatatype (buf, cur_node->children);
	    }
	  else if (strcmp ("Time", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\t\tTime:\n");
	      print_intlongstrdatatype (buf, cur_node->children);
	    }
	  else if (strcmp ("TimeInterval", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\t\tTimeInterval:\n");
	      print_intlongstrdatatype (buf, cur_node->children);
	    }
	  else if (strcmp ("Counter", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\t\tCounter:\n");
	      print_intlongstrdatatype (buf, cur_node->children);
	    }
	  else
	    buffer_addf (buf, "warning: unsupported Data member '%s'\n",
			 name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_policy (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("StartDate", name) == 0)
	    buffer_addf (buf, "\t\t\t\tStartDate: %s\n",
			 cur_node->children->content);
	  else if (strcmp ("ExpiryDate", name) == 0)
	    buffer_addf (buf, "\t\t\t\tExpiryDate: %s\n",
			 cur_node->children->content);
	  else
	    buffer_addf (buf, "warning: unsupported Policy member '%s'\n",
			 name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_key (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("AlgorithmParameters", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\tAlgorithmParameters:\n");
	      print_algorithmparameters (buf, cur_node->children);
	    }
	  else if (strcmp ("Data", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\tData:\n");
	      print_data (buf, cur_node->children);
	    }
	  else if (strcmp ("Policy", name) == 0)
	    {
	      buffer_addz (buf, "\t\t\tPolicy:\n");
	      print_policy (buf, cur_node->children);
	    }
	  else
	    buffer_addf (buf, "warning: unsupported Key member '%s'\n",
			 name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_keypackage (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("DeviceInfo", name) == 0)
	    {
	      buffer_addz (buf, "\t\tDeviceInfo:\n");
	      print_deviceinfo (buf, cur_node->children);
	    }
	  else if (strcmp ("Key", name) == 0)
	    {
	      xmlAttr *cur_attr = NULL;
	      buffer_addz (buf, "\t\tKey:\n");

	      for (cur_attr = cur_node->properties; cur_attr;
		   cur_attr = cur_attr->next)
		{
		  if (strcmp ("Id", (const char *) cur_attr->name) == 0)
		    buffer_addf (buf, "\t\t\tId: %s\n",
				 (const char *) cur_attr->children->content);
		  else if (strcmp ("Algorithm", (const char *) cur_attr->name) == 0)
		    buffer_addf (buf, "\t\t\tAlgorithm: %s\n",
				 (const char *) cur_attr->children->content);
		  else
		    buffer_addf (buf, "warning: unknown attribute: %s\n",
				 (const char *) cur_attr->name);
		}
	      print_key (buf, cur_node->children);
	    }
	  else
	    buffer_addf (buf, "warning: unsupported KeyPackage member '%s'\n",
			 name);
	}
    }
}

static void
print_keypackages (struct buffer *buf, xmlNode *x)
{
  xmlNode *cur_node = NULL;
  int nth = 1;

  for (cur_node = x; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  const char *name = (const char *) cur_node->name;

	  if (strcmp ("KeyPackage", name) != 0)
	    buffer_addf (buf, "warning: expecting 'KeyPackage' found '%s'\n",
			 name);
	  else if (cur_node->properties != NULL)
	    buffer_addf (buf, "warning: KeyPackage with properties\n");
	  else if (cur_node->children == NULL)
	    buffer_addf (buf, "warning: empty KeyPackage\n");
	  else
	    {
	      buffer_addf (buf, "\tKeyPackage %d:\n", nth++);
	      print_keypackage (buf, cur_node->children);
	    }
	}
      else if (cur_node->type != XML_TEXT_NODE)
	buffer_addf (buf, "warning: unknown node type %d\n", cur_node->type);
    }
}

static void
print_keycontainer (struct buffer *buf, xmlNode *x)
{
  xmlAttr *cur_attr = NULL;
  const char *name = (const char *) x->name;

  if (strcmp ("KeyContainer", name) != 0)
    {
      buffer_addf (buf, "warning: expecting 'KeyContainer' found '%s'\n",
		   name);
      return;
    }

  if (x->next != NULL)
    buffer_addz (buf, "warning: KeyContainer has next element\n");

  if (x->properties == NULL)
    buffer_addz (buf, "warning: KeyContainer without properties\n");

  for (cur_attr = x->properties; cur_attr; cur_attr = cur_attr->next)
    {
      if (strcmp ("Version", (const char *) cur_attr->name) == 0)
	buffer_addf (buf, "\tVersion: %s\n",
		     (const char *) cur_attr->children->content);
      else if (strcmp ("Id", (const char *) cur_attr->name) == 0)
	buffer_addf (buf, "\tId: %s\n",
		     (const char *) cur_attr->children->content);
      else
	buffer_addf (buf, "warning: unknown attribute: %s\n",
		     (const char *) cur_attr->name);
    }

  print_keypackages (buf, x->children);
}

#if 0
static void
print_element_names (xmlNode * a_node, int level)
{
  xmlNode *cur_node = NULL;

  for (cur_node = a_node; cur_node; cur_node = cur_node->next)
    {
      int i;

      if (cur_node->type == XML_ELEMENT_NODE)
	{
	  for (i = 0; i < level; i++)
	    printf ("\t");
	  printf("node type: Element, name: %s\n", cur_node->name);
	}
      else if (cur_node->type != XML_TEXT_NODE)
	printf ("unknown type %d\n", cur_node->type);

      if (cur_node->properties)
	{
	  for (i = 0; i < level; i++)
	    printf ("\t");
	  printf ("attr: %s\n", cur_node->properties->name);
	}

      print_element_names(cur_node->children, level + 1);
    }
}
#endif

int
pskc_data_output (pskc_data *data,
		  pskc_data_output_formats_t format,
		  char **out, size_t *len)
{
  struct buffer buf;
  xmlNode *root_element = NULL;

  if (format != PSKC_DATA_OUTPUT_HUMAN_COMPLETE)
    return PSKC_UNKNOWN_OUTPUT_FORMAT;

  buffer_init (&buf);

  root_element = xmlDocGetRootElement(data->data);
  // print_element_names (root_element, 0);

  buffer_addz (&buf, "Portable Symmetric Key Container (PSKC):\n");

  print_keycontainer(&buf, root_element);

  buffer_getstr (&buf, out, len);
  if (*out == NULL)
    return PSKC_MALLOC_ERROR;

  return PSKC_OK;
}
