/*
 * coding.c - data encoding functions
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

#include "oath.h"

#include <stdlib.h>

#include "base32.h"
#include "c-ctype.h"

static int
hex_decode (char hex)
{
  switch (hex)
    {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
    case 'a':
      return 0x0A;
    case 'B':
    case 'b':
      return 0x0B;
    case 'C':
    case 'c':
      return 0x0C;
    case 'D':
    case 'd':
      return 0x0D;
    case 'E':
    case 'e':
      return 0x0E;
    case 'F':
    case 'f':
      return 0x0F;

    default:
      return -1;
    }

  return -1;
}

/**
 * oath_hex2bin:
 * @hexstr: input string with hex data
 * @binstr: output string that holds binary data, or NULL
 * @binlen: output variable holding needed length of @binstr
 *
 * Convert string with hex data to binary data.
 *
 * Non-hexadecimal data are not ignored but instead will lead to an
 * %OATH_INVALID_HEX error.
 *
 * If @binstr is NULL, then @binlen will be populated with the
 * necessary length.  If the @binstr buffer is too small,
 * %OATH_TOO_SMALL_BUFFER is returned and @binlen will contain the
 * necessary length.
 *
 * Returns: On success, %OATH_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
oath_hex2bin (const char *hexstr, char *binstr, size_t * binlen)
{
  bool highbits = true;
  size_t save_binlen = *binlen;
  bool too_small = false;

  *binlen = 0;

  while (*hexstr)
    {
      int val = hex_decode (*hexstr);

      if (val < 0 || val > 0x0F)
	return OATH_INVALID_HEX;

      if (binstr && save_binlen > 0)
	{
	  if (highbits)
	    *binstr = (*binstr & 0x0F) | (val << 4);
	  else
	    *binstr = (*binstr & 0xF0) | val;
	}

      hexstr++;
      if (!highbits)
	{
	  binstr++, (*binlen)++;
	  if (save_binlen > 0)
	    save_binlen--;
	  else
	    too_small = true;
	}
      highbits = !highbits;
    }

  if (!highbits)
    return OATH_INVALID_HEX;

  if (too_small)
    return OATH_TOO_SMALL_BUFFER;

  return OATH_OK;
}

/**
 * oath_bin2hex:
 * @binstr: input binary data
 * @binlen: length of input binary data @binstr
 * @hexstr: output string with hex data, must have room for 2*@binlen+1.
 *
 * Convert binary data to NUL-terminated string with hex data.  The
 * output @hexstr is allocated by the caller and must have room for at
 * least 2*@binlen+1, to make room for the encoded data and the
 * terminating NUL byte.
 *
 * Since: 1.12.0
 **/
void
oath_bin2hex (const char *binstr, size_t binlen, char *hexstr)
{
  static const char trans[] = "0123456789abcdef";

  while (binlen--)
    {
      *hexstr++ = trans[(*binstr >> 4) & 0xf];
      *hexstr++ = trans[*binstr++ & 0xf];
    }

  *hexstr = '\0';
}

/**
 * oath_base32_decode:
 * @in: input string with base32 encoded data of length @inlen
 * @inlen: length of input base32 string @in
 * @out: pointer to output variable for binary data of length @outlen, or NULL
 * @outlen: pointer to output variable holding length of @out, or NULL
 *
 * Decode a base32 encoded string into binary data.
 *
 * Space characters are ignored and pad characters are added if
 * needed.  Non-base32 data are not ignored but instead will lead to
 * an %OATH_INVALID_BASE32 error.
 *
 * The @in parameter should contain @inlen bytes of base32 encoded
 * data.  The function allocates a new string in *@out to hold the
 * decoded data, and sets *@outlen to the length of the data.
 *
 * If @out is NULL, then *@outlen will be set to what would have been
 * the length of *@out on successful encoding.
 *
 * If the caller is not interested in knowing the length of the output
 * data @out, then @outlen may be set to NULL.
 *
 * It is permitted but useless to have both @out and @outlen NULL.
 *
 * Returns: On success %OATH_OK (zero) is returned,
 *   %OATH_INVALID_BASE32 is returned if the input contains non-base32
 *   characters, and %OATH_MALLOC_ERROR is returned on memory allocation
 *   errors.
 *
 * Since: 1.12.0
 **/
int
oath_base32_decode (const char *in, size_t inlen, char **out, size_t * outlen)
{
  size_t i, j, tmplen = 0;
  char *in_upcase;
  char *tmp;
  bool ok;

  in_upcase = malloc (inlen + 6);	/* leave room for up to 6 '=' */
  if (!in_upcase)
    return OATH_MALLOC_ERROR;

  for (i = 0, j = 0; i < inlen; i++)
    {
      if (in[i] != ' ')
	in_upcase[j++] = c_toupper (in[i]);
    }

  /* add pad characters if needed */
  switch (j % 8)
    {
    case 2:
      in_upcase[j++] = '=';
      in_upcase[j++] = '=';
    case 4:
      in_upcase[j++] = '=';
    case 5:
      in_upcase[j++] = '=';
      in_upcase[j++] = '=';
    case 7:
      in_upcase[j++] = '=';
    default:
    case 0:
      break;
    }

  ok = base32_decode_alloc (in_upcase, j, &tmp, &tmplen);

  free (in_upcase);

  if (ok && !tmp)
    return OATH_MALLOC_ERROR;
  else if (!ok)
    return OATH_INVALID_BASE32;

  if (outlen)
    *outlen = tmplen;

  if (out)
    *out = tmp;
  else
    free (tmp);

  return OATH_OK;
}

/**
 * oath_base32_encode:
 * @in: input string with binary data of length @inlen
 * @inlen: length of input data @in
 * @out: pointer to newly allocated output string of length @outlen, or NULL
 * @outlen: pointer to output variable holding length of @out, or NULL
 *
 * Encode binary data into a string with base32 data.
 *
 * The @in parameter should contain @inlen bytes of data to encode.
 * The function allocates a new string in *@out to hold the encoded
 * data, and sets *@outlen to the length of the data.  The output
 * string *@out is zero-terminated (ASCII NUL), but the NUL is not
 * counted in *@outlen.
 *
 * If @out is NULL, then *@outlen will be set to what would have been
 * the length of *@out on successful encoding.
 *
 * If the caller is not interested in knowing the length of the output
 * data @out, then @outlen may be set to NULL.
 *
 * It is permitted but useless to have both @out and @outlen NULL.
 *
 * Returns: On success %OATH_OK (zero) is returned,
 *   %OATH_BASE32_OVERFLOW is returned if the output would be too large
 *   to store, and %OATH_MALLOC_ERROR is returned on memory allocation
 *   errors.
 *
 * Since: 1.12.0
 **/
int
oath_base32_encode (const char *in, size_t inlen, char **out, size_t * outlen)
{
  char *tmp;
  size_t len = base32_encode_alloc (in, inlen, &tmp);

  if (len == 0 && tmp == NULL)
    return OATH_BASE32_OVERFLOW;
  else if (tmp == NULL)
    return OATH_MALLOC_ERROR;

  if (outlen)
    *outlen = len;

  if (out)
    *out = tmp;
  else
    free (tmp);

  return OATH_OK;
}
