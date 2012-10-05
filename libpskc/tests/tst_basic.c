/*
 * tst_basic.c - self-tests for libpskc basic functions
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

#include <stdio.h>

int
main (void)
{
  pskc_rc rc;
  int i;

  /* Check version. */

  if (!pskc_check_version (PSKC_VERSION))
    {
      printf ("pskc_check_version (%s) failed [%s]\n", PSKC_VERSION,
	      pskc_check_version (NULL));
      return 1;
    }

  if (pskc_check_version (NULL) == NULL)
    {
      printf ("pskc_check_version (NULL) == NULL\n");
      return 1;
    }

  if (pskc_check_version ("999.999"))
    {
      printf ("pskc_check_version (999.999) succeeded?!\n");
      return 1;
    }

  /* Test initialization. */

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  /* Test deinitialization. */

  rc = pskc_global_done ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_done: %d\n", rc);
      return 1;
    }

  rc = pskc_global_done ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_done: %d\n", rc);
      return 1;
    }

  rc = pskc_global_done ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_done: %d\n", rc);
      return 1;
    }

  /* Test enums. */

  for (i = 0; i < PSKC_PINUSAGEMODE_LAST; i++)
    {
      pskc_pinusagemode m;
      const char *str;

      str = pskc_pinusagemode2str (i);
      if (str == NULL)
	{
	  printf ("pskc_pinusagemode2str(%d) == NULL\n", i);
	  return 1;
	}

      m = pskc_str2pinusagemode (str);
      if (m != i)
	{
	  printf ("pskc_str2pinusagemode(%s/%d) = %d\n", str, i, m);
	  return 1;
	}
    }

  for (i = 0; i < PSKC_VALUEFORMAT_LAST; i++)
    {
      pskc_valueformat m;
      const char *str;

      str = pskc_valueformat2str (i);
      if (str == NULL)
	{
	  printf ("pskc_valueformat2str(%d) == NULL\n", i);
	  return 1;
	}

      m = pskc_str2valueformat (str);
      if (m != i)
	{
	  printf ("pskc_str2valueformat(%s/%d) = %d\n", str, i, m);
	  return 1;
	}
    }

  for (i = 0; i < PSKC_KEYUSAGE_LAST; i = i << 1)
    {
      pskc_valueformat m;
      const char *str;

      str = pskc_valueformat2str (i);
      if (str == NULL)
	{
	  printf ("pskc_valueformat2str(%d) == NULL\n", i);
	  return 1;
	}

      m = pskc_str2valueformat (str);
      if (m != i)
	{
	  printf ("pskc_str2valueformat(%s/%d) = %d\n", str, i, m);
	  return 1;
	}
    }

  return 0;
}
