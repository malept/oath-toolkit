/*
 * pskc/container.h - PSKC header file with PSKC Container functions.
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

#ifndef PSKC_CONTAINER_H
#define PSKC_CONTAINER_H

/**
 * SECTION:container
 * @short_description: High-level PSKC container handling.
 *
 * PSKC data is represented through the #pskc_t type which is created
 * by calling pskc_init() and destroyed by calling pskc_done().  You
 * may parse PSKC data in XML form from a buffer by calling
 * pskc_parse_from_memory().  To convert PSKC data to human readable
 * form you may use pskc_output().  To validate PSKC data against the
 * XML Schema, you may use pskc_validate().  To generate PSKC based on
 * the internal parsed representation you may use pskc_build_xml()
 * which takes a #pskc_output_format enumeration to indicate output
 * form.
 *
 * The PSKC data structure is a high-level structure that only carries
 * a version indicator (see pskc_get_version()), an optional identity
 * field (see pskc_get_id()) and any number of #pskc_key_t types, each
 * containing one key (see pskc_get_keypackage()).
 */

extern PSKCAPI int pskc_init (pskc_t ** container);
extern PSKCAPI void pskc_done (pskc_t * container);

extern PSKCAPI int pskc_parse_from_memory (pskc_t * container,
					   size_t len, const char *buffer);

extern PSKCAPI int pskc_get_signed_p (pskc_t * container);

extern PSKCAPI int pskc_validate (pskc_t * container, int *isvalid);

extern PSKCAPI int pskc_build_xml (pskc_t * container, char **out,
				   size_t * len);

extern PSKCAPI int pskc_sign_x509 (pskc_t * container,
				   const char *key_file,
				   const char *cert_file);
extern PSKCAPI int pskc_verify_x509crt (pskc_t * container,
					const char *cert_file,
					int *valid_signature);

/* Access data fields. */
extern PSKCAPI const char *pskc_get_version (pskc_t * container);
extern PSKCAPI void pskc_set_version (pskc_t * container,
				      const char *version);
extern PSKCAPI const char *pskc_get_id (pskc_t * container);
extern PSKCAPI void pskc_set_id (pskc_t * container, const char *id);

extern PSKCAPI pskc_key_t *pskc_get_keypackage (pskc_t * container, size_t i);
extern PSKCAPI int pskc_add_keypackage (pskc_t * container,
					pskc_key_t ** key);

/**
 * pskc_output_formats_t:
 * @PSKC_OUTPUT_HUMAN_COMPLETE: All information in human-readable format.
 * @PSKC_OUTPUT_XML: Output container in XML format.
 * @PSKC_OUTPUT_INDENTED_XML: Output container in intended XML format (will
 *   invalidate any XML Digital Signatures).
 *
 * Enumeration of different PSKC output formats.
 */
typedef enum pskc_output_formats
{
  PSKC_OUTPUT_HUMAN_COMPLETE = 0,
  PSKC_OUTPUT_XML = 1,
  PSKC_OUTPUT_INDENTED_XML = 2
} pskc_output_formats_t;

extern PSKCAPI int pskc_output (pskc_t * container,
				pskc_output_formats_t format,
				char **out, size_t * len);

#endif /* PSKC_CONTAINER_H */
