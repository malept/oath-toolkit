/*
 * data.c - implementation of PSKC data handling
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

/**
 * pskc_data_init:
 * @data: pointer to #pskc_data handle to initialize
 *
 * This function initializes the PSKC data handle @data.  The memory
 * allocate can be released by calling pskc_data_done().
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned.
 **/
int
pskc_data_init (pskc_data **data)
{
  *data = calloc (1, sizeof (**data));
  if (*data == NULL)
    return PSKC_MALLOC_ERROR;
  return PSKC_OK;
}

/**
 * pskc_data_init_from_memory:
 * @data: pointer to #pskc_data handle to initialize
 * @len: length of @buffer.
 * @buffer: XML data to parse.
 *
 * This function initializes the PSKC data handle @data.  The memory
 * allocate can be released by calling pskc_data_done().  The function
 * will also parse the XML data in @buffer of @len size.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned, on XML parse
 *   errors %PSKC_XML_PARSE_ERROR is returned.
 **/
int
pskc_data_init_from_memory (pskc_data **data, size_t len, const char *buffer)
{
  xmlDocPtr xmldoc;
  pskc_data *pd;
  int rc;

  rc = pskc_data_init (&pd);
  if (rc != PSKC_OK)
    return rc;

  xmldoc = xmlParseMemory (buffer, len);
  if (xmldoc == NULL)
    {
      pskc_data_done (pd);
      return PSKC_XML_PARSE_ERROR;
    }

  pd->xmldoc = xmldoc;

  rc = _pskc_parse (pd);
  if (rc != PSKC_OK)
    {
      pskc_data_done (pd);
      return rc;
    }

  *data = pd;

  return PSKC_OK;
}

/**
 * pskc_data_done:
 * @data: #pskc_data handle to deinitialize
 *
 * This function releases the resources associated with the @data
 * handle.
 **/
void
pskc_data_done (pskc_data *data)
{
  xmlFreeDoc(data->xmldoc);
  free (data->keypackages);
  free (data);
}
