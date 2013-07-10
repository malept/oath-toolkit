/*
 * pskc/export.h - library visibility macro.
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

#ifndef PSKC_EXPORTS_H
#define PSKC_EXPORTS_H

#ifndef PSKCAPI
#if defined PSKC_BUILDING && defined HAVE_VISIBILITY && HAVE_VISIBILITY
#define PSKCAPI __attribute__((__visibility__("default")))
#elif defined PSKC_BUILDING && defined _MSC_VER && ! defined PSKC_STATIC
#define PSKCAPI __declspec(dllexport)
#elif defined _MSC_VER && ! defined PSKC_STATIC
#define PSKCAPI __declspec(dllimport)
#else
#define PSKCAPI
#endif
#endif

#endif /* PSKC_EXPORTS_H */
