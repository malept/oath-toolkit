/*
 * tst_validate.c - self-tests for libpskc validation functions
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

#include <stdio.h>
#include <string.h>

const char *pskc_mini =
  "<?xml version=\"1.0\"?>\n"
  "<KeyContainer xmlns=\"urn:ietf:params:xml:ns:keyprov:pskc\"\n"
  "              Version=\"1.0\">\n" "  <KeyPackage/>\n" "</KeyContainer>\n";

void
my_log (const char *msg)
{
  puts (msg);
}

int
main (void)
{
  pskc_t *pskc;
  pskc_key_t *pskc_key;
  char *out;
  size_t len;
  int isvalid;
  int rc;

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  pskc_global_log (my_log);

  rc = pskc_init (&pskc);
  if (rc != PSKC_OK)
    {
      printf ("pskc_init: %d\n", rc);
      return 1;
    }

  rc = pskc_parse_from_memory (pskc, strlen (pskc_mini), pskc_mini);
  if (rc != PSKC_OK)
    {
      printf ("pskc_parse_from_memory: %d\n", rc);
      return 1;
    }

  rc = pskc_validate (pskc, &isvalid);
  if (rc != PSKC_OK)
    {
      printf ("pskc_validate: %d\n", rc);
      return 1;
    }

  if (!isvalid)
    {
      printf ("schema is invalid\n");
      return 1;
    }

  pskc_done (pskc);

  pskc_global_done ();

  return 0;
}
