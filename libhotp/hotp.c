/*
 * hotp.c - implementation of the HOTP algorithm
 * Copyright (C) 2009  Simon Josefsson
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

#include "hotp.h"

#include <stdio.h>		/* For snprintf, getline. */
#include <string.h>		/* For strverscmp. */
#include <unistd.h>		/* For ssize_t. */
#include <fcntl.h>		/* For fcntl. */
#include <errno.h>		/* For errno. */

#include "gc.h"

/**
 * hotp_init - initialize the HOTP library
 *
 * This function initializes the HOTP library.  Every user of this
 * library needs to call this function before using other functions.
 * You should call hotp_done() when use of the HOTP library is no
 * longer needed.
 *
 * Note that this function may also initialize Libgcrypt, if the HOTP
 * library is built with libgcrypt support and libgcrypt has not been
 * initialized before.  Thus if you want to manually initialize
 * libgcrypt you must do it before calling this function.  This is
 * useful in cases you want to disable libgcrypt's internal lockings
 * etc.
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_init (void)
{
  if (gc_init () != GC_OK)
    return HOTP_CRYPTO_ERROR;

  return HOTP_OK;
}

/**
 * hotp_done - deinitialize the HOTP library
 *
 * This function deinitializes the HOTP library, which were
 * initialized using hotp_init().  After calling this function, no
 * other HOTP library function may be called except for to
 * re-initialize the library using hotp_init().
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_done (void)
{
  gc_done ();

  return HOTP_OK;
}

/**
 * hotp_check_version:
 * @req_version: version string to compare with, or %NULL.
 *
 * Check HOTP library version.
 *
 * See %HOTP_VERSION for a suitable @req_version string.
 *
 * This function is one of few in the library that can be used without
 * a successful call to hotp_init().
 *
 * Return value: Check that the version of the library is at
 *   minimum the one given as a string in @req_version and return the
 *   actual version string of the library; return %NULL if the
 *   condition is not met.  If %NULL is passed to this function no
 *   check is done and only the version string is returned.
 **/
const char *
hotp_check_version (const char *req_version)
{
  if (!req_version || strverscmp (req_version, HOTP_VERSION) <= 0)
    return HOTP_VERSION;

  return NULL;
}

#define maybe_lowercase(c)					\
  ((c) == 'A' ? 'a' :						\
   ((c) == 'B' ? 'b' :						\
    ((c) == 'C' ? 'c' :						\
     ((c) == 'D' ? 'd' :					\
      ((c) == 'E' ? 'e' :					\
       ((c) == 'F' ? 'f' :					\
	c))))))

/**
 * hotp_hex2bin:
 * @hexstr: input string with hex data
 * @binstr: output string that holds binary data, or %NULL
 * @binlen: output variable holding needed length of @binstr
 *
 * Convert string with hex data to binary data.
 *
 * Non-hexadecimal data are not ignored but instead will lead to an
 * %HOTP_INVALID_HEX error.
 *
 * If @binstr is %NULL, then @binlen will be populated with the
 * necessary length.  If the @binstr buffer is too small,
 * %HOTP_TOO_SMALL_BUFFER is returned and @binlen will contain the
 * necessary length.
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_hex2bin (char *hexstr, char *binstr, size_t * binlen)
{
  const char *hexalphabet = "0123456789abcdef";
  bool highbits = true;
  size_t save_binlen = *binlen;
  bool too_small = false;

  *binlen = 0;

  while (*hexstr)
    {
      char *p;

      p = strchr (hexalphabet, maybe_lowercase (*hexstr));
      if (!p)
	return HOTP_INVALID_HEX;

      if (binstr && save_binlen > 0)
	{
	  if (highbits)
	    *binstr = (*binstr & 0x0F) | ((p - hexalphabet) << 4);
	  else
	    *binstr = (*binstr & 0xF0) | (p - hexalphabet);
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
    return HOTP_INVALID_HEX;

  if (too_small)
    return HOTP_TOO_SMALL_BUFFER;

  return HOTP_OK;
}

/**
 * hotp_generate_otp:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @moving_factor: a counter indicating the current OTP to generate
 * @digits: number of requested digits in the OTP, excluding checksum
 * @add_checksum: whether to add a checksum digit or not
 * @truncation_offset: use a specific truncation offset
 * @output_otp: output buffer, must have room for the output OTP plus zero
 *
 * Generate a one-time-password using the HOTP algorithm as described
 * in RFC 4226.
 *
 * Use a value of %HOTP_DYNAMIC_TRUNCATION for @truncation_offset
 * unless you really need a specific truncation offset.
 *
 * To find out the size of the OTP you may use the HOTP_OTP_LENGTH()
 * macro.  The @output_otp buffer must be have room for that length
 * plus one for the terminating NUL.
 *
 * Currently only values 6, 7 and 8 for @digits are supported, and the
 * @add_checksum value is ignored.  These restrictions may be lifted
 * in future versions, although some limitations are inherent in the
 * protocol.
 *
 * Returns: On success, %HOTP_OK (zero) is returned, otherwise an
 *   error code is returned.
 **/
int
hotp_generate_otp (const char *secret,
		   size_t secret_length,
		   uint64_t moving_factor,
		   unsigned digits,
		   bool add_checksum,
		   size_t truncation_offset, char *output_otp)
{
  char hs[GC_SHA1_DIGEST_SIZE];
  long S;

  {
    char counter[sizeof (moving_factor)];
    size_t i;
    Gc_rc rc;

    for (i = 0; i < sizeof (counter); i++)
      counter[i] =
	(moving_factor >> ((sizeof (moving_factor) - i - 1) * 8)) & 0xFF;

    rc = gc_hmac_sha1 (secret, secret_length,
		       counter, sizeof (moving_factor), hs);
    if (rc != GC_OK)
      return HOTP_CRYPTO_ERROR;
  }

  {
    uint8_t offset = hs[sizeof (hs) - 1] & 0x0f;

    S = (((hs[offset] & 0x7f) << 24)
	 | ((hs[offset + 1] & 0xff) << 16)
	 | ((hs[offset + 2] & 0xff) << 8) | ((hs[offset + 3] & 0xff)));

#if DEBUG
    printf ("offset is %d hash is ", offset);
    for (i = 0; i < 20; i++)
      printf ("%02x ", hs[i] & 0xFF);
    printf ("\n");

    printf ("value: %d\n", S);
#endif
  }

  switch (digits)
    {
    case 6:
      S = S % 1000000;
      break;

    case 7:
      S = S % 10000000;
      break;

    case 8:
      S = S % 100000000;
      break;

    default:
      return HOTP_INVALID_DIGITS;
      break;
    }

  {
    int len = snprintf (output_otp, digits + 1, "%.*ld", digits, S);
    output_otp[digits] = '\0';
    if (len != digits)
      return HOTP_PRINTF_ERROR;
  }

  return HOTP_OK;
}

/**
 * hotp_validate_otp:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @start_moving_factor: start counter in OTP stream
 * @window: how many OTPs from start counter to test
 * @otp: the OTP to validate.
 *
 * Validate an OTP according to OATH HOTP algorithm per RFC 4226.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns position in OTP window (zero is first position),
 *   or %HOTP_INVALID_OTP if no OTP was found in OTP window, or an
 *   error code.
 **/
int
hotp_validate_otp (const char *secret,
		   size_t secret_length,
		   uint64_t start_moving_factor,
		   size_t window, const char *otp)
{
  unsigned digits = strlen (otp);
  unsigned iter = 0;
  char tmp_otp[10];
  int rc;

  do
    {
      rc = hotp_generate_otp (secret,
			      secret_length,
			      start_moving_factor + iter,
			      digits,
			      false, HOTP_DYNAMIC_TRUNCATION, tmp_otp);
      if (rc != HOTP_OK)
	return rc;

      if (strcmp (otp, tmp_otp) == 0)
	return iter;
    }
  while (window - iter++ > 0);

  return HOTP_INVALID_OTP;
}

static unsigned
parse_type (const char *str)
{
  if (strcmp (str, "HOTP/E/6") == 0)
    return 6;
  if (strcmp (str, "HOTP/E/7") == 0)
    return 7;
  if (strcmp (str, "HOTP/E/8") == 0)
    return 8;
  if (strcmp (str, "HOTP/E") == 0)
    return 6;
  if (strcmp (str, "HOTP") == 0)
    return 6;
  return 0;
}

static const char *whitespace = " \t\r\n";
static const char *time_format_string = "%Y-%m-%dT%H:%M:%SL";

static int
parse_usersfile (const char *username,
		 const char *otp,
		 size_t window,
		 const char *passwd,
		 time_t * last_otp,
		 FILE * infh,
		 char **lineptr,
		 size_t * n,
		 uint64_t *new_moving_factor)
{
  ssize_t t;

  while ((t = getline (lineptr, n, infh)) != -1)
    {
      char *saveptr;
      char *p = strtok_r (*lineptr, whitespace, &saveptr);
      unsigned digits;
      char secret[20];
      size_t secret_length = sizeof (secret);
      uint64_t start_moving_factor = 0;
      int rc;
      char *prev_otp = NULL;

      if (p == NULL)
	continue;

      /* Read token type */
      digits = parse_type (p);
      if (digits == 0)
	continue;

      /* Read username */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p == NULL || strcmp (p, username) != 0)
	continue;

      /* Read password. */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (passwd)
	{
	  if (p == NULL)
	    continue;
	  if (strcmp (p, "-") == 0 && *p != '\0')
	    return HOTP_BAD_PASSWORD;
	  if (strcmp (p, passwd) != 0)
	    return HOTP_BAD_PASSWORD;
	}

      /* Read key. */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p == NULL)
	continue;
      rc = hotp_hex2bin (p, secret, &secret_length);
      if (rc != HOTP_OK)
	return rc;

      /* Read (optional) moving factor. */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p && *p)
	{
	  char *endptr;
	  unsigned long long int ull = strtoull (p, &endptr, 10);
	  if (endptr && *endptr != '\0')
	    return HOTP_INVALID_COUNTER;
	  start_moving_factor = ull;
	}

      /* Read (optional) last OTP */
      prev_otp = strtok_r (NULL, whitespace, &saveptr);

      /* Read (optional) last_otp */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p)
	{
	  struct tm tm;
	  char *t;

	  t = strptime (p, time_format_string, &tm);
	  if (t == NULL || *t != '\0')
	    return HOTP_INVALID_TIMESTAMP;
	  tm.tm_isdst = -1;
	  if (last_otp)
	    {
	      *last_otp = mktime (&tm);
	      if (*last_otp == (time_t) - 1)
		return HOTP_INVALID_TIMESTAMP;
	    }
	}

      if (prev_otp && strcmp (prev_otp, otp) == 0)
	return HOTP_REPLAYED_OTP;

      rc = hotp_validate_otp (secret, secret_length,
			      start_moving_factor, window, otp);
      if (rc < 0)
	return rc;
      *new_moving_factor = start_moving_factor + rc;
      return HOTP_OK;
    }

  return HOTP_UNKNOWN_USER;
}

static int
update_usersfile2 (const char *username,
		   const char *otp,
		   FILE * infh,
		   FILE * outfh,
		   char **lineptr,
		   size_t * n,
		   char *timestamp,
		   uint64_t new_moving_factor)
{
  ssize_t t;

  while ((t = getline (lineptr, n, infh)) != -1)
    {
      char *saveptr;
      char *origline;
      char *user, *type, *passwd, *secret;
      int r;

      origline = strdup (*lineptr);

      type = strtok_r (*lineptr, whitespace, &saveptr);
      if (type == NULL)
	continue;

      /* Read username */
      user = strtok_r (NULL, whitespace, &saveptr);
      if (user == NULL || strcmp (user, username) != 0)
	{
	  r = fputs (origline, outfh);
	  if (r <= 0)
	    return HOTP_PRINTF_ERROR;
	  continue;
	}

      passwd = strtok_r (NULL, whitespace, &saveptr);
      if (passwd == NULL)
	passwd = "-";

      secret = strtok_r (NULL, whitespace, &saveptr);
      if (secret == NULL)
	secret = "-";

      r = fprintf (outfh, "%s\t%s\t%s\t%s\t%llu\t%s\t%s\n",
		   type, username, passwd, secret,
		   (unsigned long long) new_moving_factor,
		   otp, timestamp);
      if (r <= 0)
	return HOTP_PRINTF_ERROR;
    }

  return HOTP_OK;
}

static int
update_usersfile (const char *usersfile,
		  const char *username,
		  const char *otp,
		  time_t *last_otp,
		  FILE * infh,
		  char **lineptr,
		  size_t * n,
		  char *timestamp,
		  uint64_t new_moving_factor)
{
  FILE *outfh, *lockfh;
  int rc;
  char *newfilename, *lockfile;

  /* Rewind input file. */
  {
    int pos;

    pos = fseek (infh, 0L, SEEK_SET);
    if (pos == -1)
      return HOTP_FILE_SEEK_ERROR;
    clearerr (infh);
  }

  /* Open lockfile. */
  {
    int l;

    l = asprintf (&lockfile, "%s.lock", usersfile);
    if (lockfile == NULL || l != strlen (usersfile) + 5)
      return HOTP_PRINTF_ERROR;

    lockfh = fopen (lockfile, "w");
    if (!lockfh)
      {
	free (lockfile);
	return HOTP_FILE_CREATE_ERROR;
      }
  }

  /* Lock the lockfile. */
  {
    struct flock l;

    memset (&l, 0, sizeof (l));
    l.l_whence = SEEK_SET;
    l.l_start = 0;
    l.l_len = 0;
    l.l_type = F_WRLCK;

    while ((rc = fcntl (fileno (lockfh), F_SETLKW, &l)) < 0 && errno == EINTR)
      continue;
    if (rc == -1)
      {
	fclose (lockfh);
	free (lockfile);
	return HOTP_FILE_LOCK_ERROR;
      }
  }

  /* Open the "new" file. */
  {
    int l;

    l = asprintf (&newfilename, "%s.new", usersfile);
    if (newfilename == NULL || l != strlen (usersfile) + 4)
      {
	fclose (lockfh);
	free (lockfile);
	return HOTP_PRINTF_ERROR;
      }

    outfh = fopen (newfilename, "w");
    if (!outfh)
      {
	free (newfilename);
	fclose (lockfh);
	free (lockfile);
	return HOTP_FILE_CREATE_ERROR;
      }
  }

  rc = update_usersfile2 (username, otp, infh, outfh, lineptr, n,
			  timestamp, new_moving_factor);

  fclose (lockfh);
  fclose (outfh);

  {
    int tmprc1, tmprc2;

    tmprc1 = rename (newfilename, usersfile);
    free (newfilename);

    tmprc2 = unlink (lockfile);
    free (lockfile);

    if (tmprc1 == -1)
      return HOTP_FILE_RENAME_ERROR;
    if (tmprc2 == -1)
      return HOTP_FILE_UNLINK_ERROR;
  }

  return rc;
}

/**
 * hotp_authenticate_usersfile:
 * @usersfile: string with user credential filename, in UsersFile format
 * @username: string with name of user
 * @otp: string with one-time password to authenticate
 * @window: how many future OTPs to search
 * @passwd: string with password, or %NULL to disable password checking
 * @last_otp: output variable holding last successful authentication
 *
 * Authenticate user named @username with the one-time password @otp
 * and (optional) password @passwd.  Credentials are read (and
 * updated) from a text file named @usersfile.
 *
 * Returns: On successful validation, %HOTP_OK is returned.  If the
 *   supplied @otp is the same as the last successfully authenticated
 *   one-time password, %HOTP_REPLAYED_OTP is returned and the
 *   timestamp of the last authentication is returned in @last_otp.
 *   If the one-time password is not found in the indicated search
 *   window, %HOTP_INVALID_OTP is returned.  Otherwise, an error code
 *   is returned.
 **/
int
hotp_authenticate_usersfile (const char *usersfile,
			     const char *username,
			     const char *otp,
			     size_t window,
			     const char *passwd,
			     time_t * last_otp)
{
  FILE *infh;
  char *line = NULL;
  size_t n = 0;
  uint64_t new_moving_factor;
  int rc;

  infh = fopen (usersfile, "r");
  if (!infh)
    return HOTP_NO_SUCH_FILE;

  rc = parse_usersfile (username, otp, window, passwd, last_otp,
			infh, &line, &n, &new_moving_factor);

  if (rc == HOTP_OK)
    {
      char timestamp[30];
      size_t max = sizeof (timestamp);
      struct tm now;
      time_t t;
      size_t l;
      int r;

      if (time (&t) == (time_t) -1)
	return HOTP_TIME_ERROR;

      if (localtime_r (&t, &now) == NULL)
	return HOTP_TIME_ERROR;

      l = strftime (timestamp, max, time_format_string, &now);
      if (l != 20)
	return HOTP_TIME_ERROR;

      rc = update_usersfile (usersfile, username, otp, last_otp,
			     infh, &line, &n, timestamp,
			     new_moving_factor);
    }

  free (line);
  fclose (infh);

  return rc;
}
