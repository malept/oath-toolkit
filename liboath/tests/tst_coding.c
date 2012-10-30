/*
 * tst_coding.c - self-tests for liboath data coding functions
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
#include <stdlib.h>

int
main (void)
{
  oath_rc rc;
  char *hexsecret = "ABCDEF3435363738393031323334353637abcdef";
  char secret[100];
  size_t secretlen;
  char *tmp;
  size_t len;

  /* Hex decoding. */

  secretlen = 0;
  rc = oath_hex2bin (hexsecret, secret, &secretlen);
  if (rc != OATH_TOO_SMALL_BUFFER)
    {
      printf ("oath_hex2bin too small: %d\n", rc);
      return 1;
    }
  if (secretlen != 20)
    {
      printf ("oath_hex2bin too small: 20 != %ld\n", secretlen);
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
      printf ("oath_hex2bin: 20 != %ld\n", secretlen);
      return 1;
    }
  if (memcmp (secret, "\xAB\xCD\xEF\x34\x35\x36\x37\x38\x39\x30"
	      "\x31\x32\x33\x34\x35\x36\x37\xab\xcd\xef", 20) != 0)
    {
      printf ("oath_hex2bin: decode mismatch\n");
      return 1;
    }

  /* Hex decoding */

  oath_bin2hex (NULL, 0, secret);
  oath_bin2hex ("", 0, secret);

  oath_bin2hex ("x", 1, secret);
  if (strcmp (secret, "78") != 0)
    {
      printf ("oath_bin2hex: decode mismatch\n");
      return 1;
    }
  oath_bin2hex ("xx", 2, secret);
  if (strcmp (secret, "7878") != 0)
    {
      printf ("oath_bin2hex: decode mismatch\n");
      return 1;
    }

  /* Base32 encoding. */

  rc = oath_base32_encode (NULL, 0, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_encode ("", 0, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_encode ("", 1, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_encode ("foo", 3, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_encode ("foo", 3, NULL, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  if (len != strlen ("MZXW6==="))
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_encode ("foo", 3, &tmp, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  if (strcmp (tmp, "MZXW6===") != 0)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  free (tmp);

  len = 42;
  rc = oath_base32_encode ("foo", 3, &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  if (len != strlen ("MZXW6===") || strcmp (tmp, "MZXW6===") != 0)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  free (tmp);

  len = 42;
  rc = oath_base32_encode ("foobar", 6, &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  if (len != strlen ("MZXW6YTBOI======")
      || strcmp (tmp, "MZXW6YTBOI======") != 0)
    {
      printf ("oath_base32_encode: %d\n", rc);
      return 1;
    }
  free (tmp);

  /* Base32 decoding. */

  rc = oath_base32_decode (NULL, 0, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_decode ("", 0, NULL, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_decode ("", 1, NULL, NULL);
  if (rc != OATH_INVALID_BASE32)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_decode ("NIXnix", 6, &tmp, &len);
  if (rc != OATH_INVALID_BASE32)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }

  rc = oath_base32_decode ("MZXW6===", 8, NULL, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != strlen ("foo"))
    {
      printf ("oath_base32_decode length mismatch: %ld\n", len);
      return 1;
    }

  rc = oath_base32_decode ("mzxw6===", 8, NULL, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != strlen ("foo"))
    {
      printf ("oath_base32_decode length mismatch: %ld\n", len);
      return 1;
    }

  rc = oath_base32_decode ("MZXW6===", 8, &tmp, NULL);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (memcmp (tmp, "foo", 3) != 0)
    {
      printf ("oath_base32_decode strcmp failure: %s\n", tmp);
      return 1;
    }
  free (tmp);

  rc = oath_base32_decode ("MZXW6===", 8, &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != 3 || memcmp (tmp, "foo", len) != 0)
    {
      printf ("oath_base32_decode failure: %ld/%s\n", len, tmp);
      return 1;
    }
  free (tmp);

#define FOOBAR "MZXW6YTBOI======"
  rc = oath_base32_decode (FOOBAR, strlen (FOOBAR), &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != 6 || memcmp (tmp, "foobar", len) != 0)
    {
      printf ("oath_base32_decode failure: %ld/%s\n", len, tmp);
      return 1;
    }
  free (tmp);

  rc = oath_base32_decode ("MZ XW 6===", 10, &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != 3 || memcmp (tmp, "foo", len) != 0)
    {
      printf ("oath_base32_decode failure: %ld/%s\n", len, tmp);
      return 1;
    }
  free (tmp);

  rc = oath_base32_decode ("MZ XW 6", 7, &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != 3 || memcmp (tmp, "foo", len) != 0)
    {
      printf ("oath_base32_decode failure: %ld/%s\n", len, tmp);
      return 1;
    }
  free (tmp);

#define DROPBOX "gr6d 5br7 25s6 vnck v4vl hlao re"
  rc = oath_base32_decode (DROPBOX, strlen (DROPBOX), &tmp, &len);
  if (rc != OATH_OK)
    {
      printf ("oath_base32_decode: %d\n", rc);
      return 1;
    }
  if (len != 16)
    {
      printf ("oath_base32_decode failure: %ld\n", len);
      return 1;
    }
  free (tmp);

  return 0;
}
