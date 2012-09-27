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

/**
 * pskc_init:
 * @container: pointer to #pskc handle to initialize
 *
 * This function initializes the PSKC @container handle.  The memory
 * allocate can be released by calling pskc_done().
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned.
 **/
int
pskc_init (pskc ** container)
{
  *container = calloc (1, sizeof (**container));
  if (*container == NULL)
    return PSKC_MALLOC_ERROR;
  return PSKC_OK;
}

/**
 * pskc_init_from_memory:
 * @container: pointer to #pskc handle to initialize
 * @len: length of @buffer.
 * @buffer: XML data to parse.
 *
 * This function initializes the PSKC @container handle.  The memory
 * allocate can be released by calling pskc_done().  The function will
 * also parse the XML data in @buffer of @len size.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, on memory
 *   allocation errors %PSKC_MALLOC_ERROR is returned, on XML parse
 *   errors %PSKC_XML_PARSE_ERROR is returned.
 **/
int
pskc_init_from_memory (pskc ** container, size_t len, const char *buffer)
{
  xmlDocPtr xmldoc;
  int rc;

  rc = pskc_init (container);
  if (rc != PSKC_OK)
    return rc;

  xmldoc = xmlParseMemory (buffer, len);
  if (xmldoc == NULL)
    {
      pskc_done (*container);
      return PSKC_XML_PARSE_ERROR;
    }

  (*container)->xmldoc = xmldoc;

  rc = _pskc_parse (*container);
  if (rc != PSKC_OK)
    {
      pskc_done (*container);
      return rc;
    }

  return PSKC_OK;
}

/**
 * pskc_done:
 * @container: #pskc handle to deinitialize
 *
 * This function releases the resources associated with the PSKC
 * @container handle.
 **/
void
pskc_done (pskc * container)
{
  xmlFreeDoc (container->xmldoc);
  free (container->keypackages);
  free (container);
}
