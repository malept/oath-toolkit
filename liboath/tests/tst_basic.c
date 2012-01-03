/*
 * tst_basic.c - self-tests for liboath basic functions
 * Copyright (C) 2009-2012 Simon Josefsson
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
  char *hexsecret = "ABCDEF3435363738393031323334353637abcdef";
  char secret[20];
  size_t secretlen;

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

  rc = oath_init ();
  if (rc != OATH_OK)
    {
      printf ("oath_init: %d\n", rc);
      return 1;
    }

  secretlen = 0;
  rc = oath_hex2bin (hexsecret, secret, &secretlen);
  if (rc != OATH_TOO_SMALL_BUFFER)
    {
      printf ("oath_hex2bin too small: %d\n", rc);
      return 1;
    }
  if (secretlen != 20)
    {
      printf ("oath_hex2bin too small: 20 != %d\n", secretlen);
      return 1;
    }

  rc = oath_hex2bin ("abcd", secret, &secretlen);
  if (rc != OATH_OK)
    {
      printf ("oath_hex2bin lower case failed: %d\n", rc);
      return 1;
    }

  rc = oath_hex2bin ("ABCD", secret, &secretlen);
  if (rc != OATH_OK)
    {
      printf ("oath_hex2bin upper case failed: %d\n", rc);
      return 1;
    }

  rc = oath_hex2bin ("ABC", secret, &secretlen);
  if (rc != OATH_INVALID_HEX)
    {
      printf ("oath_hex2bin too small failed: %d\n", rc);
      return 1;
    }

  rc = oath_hex2bin ("JUNK", secret, &secretlen);
  if (rc != OATH_INVALID_HEX)
    {
      printf ("oath_hex2bin junk failed: %d\n", rc);
      return 1;
    }

  secretlen = sizeof (secret);
  rc = oath_hex2bin (hexsecret, secret, &secretlen);
  if (rc != OATH_OK)
    {
      printf ("oath_hex2bin: %d\n", rc);
      return 1;
    }
  if (secretlen != 20)
    {
      printf ("oath_hex2bin: 20 != %d\n", secretlen);
      return 1;
    }
  if (memcmp (secret, "\xAB\xCD\xEF\x34\x35\x36\x37\x38\x39\x30"
	      "\x31\x32\x33\x34\x35\x36\x37\xab\xcd\xef", 20) != 0)
    {
      printf ("oath_hex2bin: decode mismatch\n");
      return 1;
    }

  rc = oath_done ();
  if (rc != OATH_OK)
    {
      printf ("oath_done: %d\n", rc);
      return 1;
    }

  return 0;
}
