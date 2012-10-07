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

/**
 * SECTION:global
 * @short_description: Global functions.
 *
 * The library is initialized using pskc_global_init() which is a
 * thread-unsafe function that should be called when the code that
 * needs the PSKC library functionality is initialized.  When the
 * application no longer needs to use the PSKC Library, it can call
 * pskc_global_done() to release resources.
 *
 * The pskc_free() function is used to de-allocate memory that was
 * allocated by the library earlier and returned to the caller.
 *
 * For debugging, you can implement a function of the #pskc_log_func
 * signature and call pskc_global_log() to make the library output
 * some messages that may provide additional information.
 */

extern PSKCAPI int pskc_global_init (void);
extern PSKCAPI void pskc_global_done (void);

typedef void (*pskc_log_func) (const char *msg);
extern PSKCAPI void pskc_global_log (pskc_log_func log_func);

extern PSKCAPI void pskc_free (void *ptr);

#endif /* PSKC_GLOBAL_H */
