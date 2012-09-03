/*
 * tst_errors.c - self-tests for liboath strerror functions
 * Copyright (C) 2011-2012 Simon Josefsson
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
  signed i;

  for (i = 3; i >= OATH_LAST_ERROR - 3; i--)
    {
      const char *name = oath_strerror_name (i);
      const char *err = oath_strerror (i);

      if ((i <= 0 && i >= OATH_LAST_ERROR) && name == NULL)
	{
	  printf ("No error string for return code %d\n", i);
	  return 1;
	}
      if ((i > 0 || i < OATH_LAST_ERROR) && name != NULL)
	{
	  printf ("Error string for unknown return code %d\n", i);
	  return 1;
	}

      printf ("%d: %s: %s\n", i, name, err);
    }

  return 0;
}
