/*
 * validate.c - Validate PSKC data according to XML Schema.
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

#include <pskc/pskc.h>

#define INTERNAL_NEED_PSKC_STRUCT
#include "internal.h"
#include <libxml/xmlschemas.h>

#define PSKC_SCHEMA_URI "urn:ietf:params:xml:ns:keyprov:pskc"

/**
 * pskc_validate:
 * @container: a #pskc_t handle, from pskc_init().
 * @isvalid: output variable holding validation result, non-0 for valid.
 *
 * This function validate the PSKC @container handle the PSKC XML
 * Schema.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, or an error code.
 **/
int
pskc_validate (pskc_t * container, int *isvalid)
{
  xmlSchemaParserCtxtPtr _pskc_parser_ctxt = NULL;
  xmlSchemaPtr _pskc_schema = NULL;
  xmlSchemaValidCtxtPtr _pskc_schema_validctxt = NULL;

  _pskc_parser_ctxt = xmlSchemaNewParserCtxt (PSKC_SCHEMA_URI);
  if (_pskc_parser_ctxt == NULL)
    {
      _pskc_debug ("xmlSchemaNewDocParserCtxt failed\n");
      return PSKC_XML_ERROR;
    }

  _pskc_schema = xmlSchemaParse (_pskc_parser_ctxt);
  if (_pskc_schema == NULL)
    {
      _pskc_debug ("xmlSchemaParse failed\n");
      xmlSchemaFreeParserCtxt (_pskc_parser_ctxt);
      return PSKC_XML_ERROR;
    }

  _pskc_schema_validctxt = xmlSchemaNewValidCtxt (_pskc_schema);
  if (_pskc_schema_validctxt == NULL)
    {
      _pskc_debug ("xmlSchemaNewValidCtxt failed\n");
      xmlSchemaFree (_pskc_schema);
      xmlSchemaFreeParserCtxt (_pskc_parser_ctxt);
      return PSKC_XML_ERROR;
    }

  *isvalid = xmlSchemaValidateDoc (_pskc_schema_validctxt,
				   container->xmldoc) == 0;

  xmlSchemaFreeValidCtxt (_pskc_schema_validctxt);
  xmlSchemaFree (_pskc_schema);
  xmlSchemaFreeParserCtxt (_pskc_parser_ctxt);

  return PSKC_OK;
}
