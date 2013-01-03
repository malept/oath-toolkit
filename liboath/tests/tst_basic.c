/*
 * tst_basic.c - self-tests for liboath basic functions
 * Copyright (C) 2009-2013 Simon Josefsson
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

#include "oath.h"

#include <stdio.h>

int
main (void)
{
  oath_rc rc;

  /* Check version. */

  if (!oath_check_version (OATH_VERSION))
    {
      printf ("oath_check_version (%s) failed [%s]\n", OATH_VERSION,
	      oath_check_version (NULL));
      return 1;
    }

  if (oath_check_version (NULL) == NULL)
    {
      printf ("oath_check_version (NULL) == NULL\n");
      return 1;
    }

  if (oath_check_version ("999.999"))
    {
      printf ("oath_check_version (999.999) succeeded?!\n");
      return 1;
    }

  /* Test initialization. */

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  /* Test deinitialization. */

  rc = oath_done ();
  if (rc != OATH_OK)
    {
      printf ("oath_done: %d\n", rc);
      return 1;
    }

  return 0;
}
