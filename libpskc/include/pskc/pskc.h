/*
 * pskc/pskc.h - PSKC header file with everything.
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

#ifndef PSKC_H
#define PSKC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PSKCAPI
# if defined PSKC_BUILDING && defined HAVE_VISIBILITY && HAVE_VISIBILITY
#  define PSKCAPI __attribute__((__visibility__("default")))
# elif defined PSKC_BUILDING && defined _MSC_VER && ! defined PSKC_STATIC
#  define PSKCAPI __declspec(dllexport)
# elif defined _MSC_VER && ! defined PSKC_STATIC
#  define PSKCAPI __declspec(dllimport)
# else
#  define PSKCAPI
# endif
#endif

#include <stddef.h> /* size_t */
#include <stdint.h> /* uint32_t, uint64_t */
#include <time.h> /* struct tm */

typedef struct pskc pskc_t;
typedef struct pskc_key pskc_key_t;

#include <pskc/version.h>
#include <pskc/errors.h>
#include <pskc/global.h>
#include <pskc/container.h>
#include <pskc/enums.h>
#include <pskc/keypackage.h>

#ifdef __cplusplus
}
#endif

#endif /* PSKC_H */
