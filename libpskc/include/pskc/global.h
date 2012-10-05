/*
 * pskc/global.h - PSKC header file with library global function prototypes.
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

#ifndef PSKC_GLOBAL_H
#define PSKC_GLOBAL_H

extern PSKCAPI int pskc_global_init (void);
extern PSKCAPI int pskc_global_done (void);

typedef void (*pskc_log_func) (const char *msg);
extern PSKCAPI void pskc_global_log (pskc_log_func log_func);

extern PSKCAPI const char *pskc_check_version (const char *req_version);

extern PSKCAPI void pskc_free (void *ptr);

#endif /* PSKC_GLOBAL_H */
