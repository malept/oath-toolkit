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

/* global.c */
extern xmlSchemaValidCtxtPtr _pskc_schema_validctxt;

/**
 * pskc_validate:
 * @container: #pskc_t handle to validate
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
  *isvalid = xmlSchemaValidateDoc (_pskc_schema_validctxt,
				   container->xmldoc) == 0;

  return PSKC_OK;
}
