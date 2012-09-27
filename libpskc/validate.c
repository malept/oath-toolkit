/*
 * validate.c - validate PSKC data according to XML Schema
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
#include <libxml/xmlschemas.h>

#define PSKC_SCHEMA_URL \
  "http://www.iana.org/assignments/xml-registry/schema/keyprov/pskc.xsd"

/**
 * pskc_validate:
 * @container: #pskc handle to validate
 * @isvalid: output variable holding validation result.
 *
 * This function validate the PSKC @container handle the PSKC XML
 * Schema.  Note that this function may need to download data from the
 * Internet.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, or an error code.
 **/
int
pskc_validate (pskc * container, int *isvalid)
{
  xmlSchemaParserCtxtPtr parser_ctxt =
    xmlSchemaNewParserCtxt (PSKC_SCHEMA_URL);
  if (parser_ctxt == NULL)
    return PSKC_XML_PARSE_ERROR;

  xmlSchemaPtr schema = xmlSchemaParse (parser_ctxt);
  if (schema == NULL)
    {
      xmlSchemaFreeParserCtxt (parser_ctxt);
      return PSKC_XML_PARSE_ERROR;
    }

  xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt (schema);
  if (valid_ctxt == NULL)
    {
      xmlSchemaFree (schema);
      xmlSchemaFreeParserCtxt (parser_ctxt);
      return PSKC_XML_PARSE_ERROR;
    }

  *isvalid = xmlSchemaValidateDoc (valid_ctxt, container->xmldoc) == 0;

  xmlSchemaFreeValidCtxt (valid_ctxt);
  xmlSchemaFree (schema);
  xmlSchemaFreeParserCtxt (parser_ctxt);

  return PSKC_OK;
}
