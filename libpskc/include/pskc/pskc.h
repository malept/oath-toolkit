/*
 * pskc/pskc.h - PSKC header file with everything.
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

#ifndef PSKC_H
#define PSKC_H

/**
 * SECTION:pskc
 * @short_description: Top-level include file.
 *
 * The top-level &lt;pskc/pskc.h&gt; include file is responsible for
 * declaring top-level types and including all other header files.
 * The #pskc_t type is used for the high-level PSKC container type and
 * the #pskc_key_t type represent each key package within the
 * container.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>		/* size_t */
#include <stdint.h>		/* uint32_t, uint64_t */
#include <time.h>		/* struct tm */

/**
 * pskc_t:
 *
 * All PSKC data is represented through the #pskc_t container type,
 * which is a high-level structure that only carries a version
 * indicator (see pskc_get_version()), an optional identity field (see
 * pskc_get_id()) and any number of #pskc_key_t types, each containing
 * one key (see pskc_get_keypackage()).
 */
  typedef struct pskc pskc_t;

/**
 * pskc_key_t:
 *
 * PSKC keys are represented through the #pskc_key_t type.  Each key
 * is part of a higher level #pskc_t container type.  The
 * pskc_get_keypackage() function is used to retrieve the #pskc_key_t
 * values from the #pskc_t structure.
 */
  typedef struct pskc_key pskc_key_t;

#include <pskc/exports.h>
#include <pskc/version.h>
#include <pskc/errors.h>
#include <pskc/global.h>
#include <pskc/container.h>
#include <pskc/enums.h>
#include <pskc/keypackage.h>

#ifdef __cplusplus
}
#endif

#endif				/* PSKC_H */
