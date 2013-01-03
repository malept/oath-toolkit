/*
 * global.c - Implementation of PSKC library global functions.
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

#include <config.h>

#include <pskc/pskc.h>

#include "internal.h"
#include <string.h>		/* strverscmp */
#include <libxml/parser.h>	/* xmlInitParser */
#include <libxml/catalog.h>	/* xmlLoadCatalog */
#include <xmlsec/xmlsec.h>
#include <xmlsec/crypto.h>

int _pskc_init = 0;

/**
 * pskc_global_init:
 *
 * This function initializes the PSKC library.  Every user of this
 * library needs to call this function before using other functions.
 * You should call pskc_global_done() when use of the PSKC library is
 * no longer needed.
 *
 * Returns: On success, %PSKC_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
pskc_global_init (void)
{
  if (_pskc_init++)
    return PSKC_OK;

  xmlInitParser ();

  xmlInitializeCatalog ();
  if (xmlLoadCatalog (PSKC_SCHEMA_CATALOG) < 0)
    /* Don't return here, let's treat not finding the catalog a
       non-fatal error.  The pskc_validate function will not work
       properly, but it will return an error in this case. */
    _pskc_debug ("xmlLoadCatalog failed");

#ifdef USE_XMLSEC
  if (xmlSecInit () < 0)
    {
      _pskc_debug ("xmlSecInit failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecCheckVersion () != 1)
    {
      _pskc_debug ("xmlSecCheckVersion failed");
      return PSKC_XMLSEC_ERROR;
    }

#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
  if (xmlSecCryptoDLLoadLibrary (BAD_CAST XMLSEC_CRYPTO) < 0)
    {
      _pskc_debug ("xmlSecCryptoDLLoadLibrary failed");
      return PSKC_XMLSEC_ERROR;
    }
#endif

  if (xmlSecCryptoAppInit (NULL) < 0)
    {
      _pskc_debug ("xmlSecCryptoAppInit failed");
      return PSKC_XMLSEC_ERROR;
    }

  if (xmlSecCryptoInit () < 0)
    {
      _pskc_debug ("xmlSecCryptoInit failed");
      return PSKC_XMLSEC_ERROR;
    }
#endif

  return PSKC_OK;
}

/**
 * pskc_global_done:
 *
 * This function deinitializes the PSKC library, which were
 * initialized using pskc_global_init().  After calling this function,
 * no other PSKC library function may be called except for to
 * re-initialize the library using pskc_global_init().
 **/
void
pskc_global_done (void)
{
  if (_pskc_init == 1)
    {
#ifdef USE_XMLSEC
      xmlSecCryptoShutdown ();
      xmlSecCryptoAppShutdown ();
      xmlSecShutdown ();
#endif
      xmlCleanupParser ();
      xmlMemoryDump ();
    }

  _pskc_init--;
}

/**
 * pskc_check_version:
 * @req_version: version string to compare with, or NULL.
 *
 * Check PSKC library version.
 *
 * See %PSKC_VERSION for a suitable @req_version string.
 *
 * This function is one of few in the library that can be used without
 * a successful call to pskc_global_init().
 *
 * Return value: Check that the version of the library is at
 *   minimum the one given as a string in @req_version and return the
 *   actual version string of the library; return NULL if the
 *   condition is not met.  If NULL is passed to this function no
 *   check is done and only the version string is returned.
 **/
const char *
pskc_check_version (const char *req_version)
{
  if (!req_version || strverscmp (req_version, PSKC_VERSION) <= 0)
    return PSKC_VERSION;

  return NULL;
}

/**
 * pskc_free:
 * @ptr: memory region to deallocate, or NULL.
 *
 * Deallocates memory region by calling free().  If @ptr is NULL no
 * operation is performed.
 *
 * This function is necessary on Windows, where different parts of the
 * same application may use different memory heaps.
 **/
void
pskc_free (void *ptr)
{
  free (ptr);
}

pskc_log_func _pskc_log_func;

/**
 * pskc_global_log:
 * @log_func: new global #pskc_log_func log function to use.
 *
 * Enable global debug logging function.  The function will be invoked
 * to print various debugging information.
 *
 * @pskc_log_func is of the form,
 * void (*pskc_log_func) (const char *msg);
 *
 * The container and keypackage variables may be NULL if they are not
 * relevant for the debug information printed.
 **/
void
pskc_global_log (pskc_log_func log_func)
{
  _pskc_log_func = log_func;
}

void
_pskc_debug (const char *format, ...)
{
  va_list ap;
  char *str;
  int rc;

  if (!_pskc_log_func)
    return;

  va_start (ap, format);
  rc = vasprintf (&str, format, ap);
  va_end (ap);

  if (rc != -1)
    {
      _pskc_log_func (str);
      free (str);
    }
}
