/*
 * usersfile.c - implementation of UsersFile based HOTP validation
 * Copyright (C) 2009-2014 Simon Josefsson
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
#undef GNULIB_POSIXCHECK	/* too many complaints for now */

#include "oath.h"

#include <stdio.h>		/* For snprintf, getline. */
#include <stdlib.h>		/* For free. */
#include <unistd.h>		/* For ssize_t. */
#include <fcntl.h>		/* For fcntl. */
#include <errno.h>		/* For errno. */
#include <sys/stat.h>		/* For S_IRUSR, S_IWUSR. */

static int
parse_type (const char *str, unsigned *digits, unsigned *totpstepsize)
{
  *totpstepsize = 0;
  if (strcmp (str, "HOTP/E/6") == 0
      || strcmp (str, "HOTP/E") == 0 || strcmp (str, "HOTP") == 0)
    *digits = 6;
  else if (strcmp (str, "HOTP/E/7") == 0)
    *digits = 7;
  else if (strcmp (str, "HOTP/E/8") == 0)
    *digits = 8;
  else if (strncmp (str, "HOTP/T30", 8) == 0)
    {
      *totpstepsize = 30;
      if (strcmp (str, "HOTP/T30") == 0 || strcmp (str, "HOTP/T30/6") == 0)
	*digits = 6;
      else if (strcmp (str, "HOTP/T30/7") == 0)
	*digits = 7;
      else if (strcmp (str, "HOTP/T30/8") == 0)
	*digits = 8;
      else
	return -1;
    }
  else if (strncmp (str, "HOTP/T60", 8) == 0)
    {
      *totpstepsize = 60;
      if (strcmp (str, "HOTP/T60") == 0 || strcmp (str, "HOTP/T60/6") == 0)
	*digits = 6;
      else if (strcmp (str, "HOTP/T60/7") == 0)
	*digits = 7;
      else if (strcmp (str, "HOTP/T60/8") == 0)
	*digits = 8;
      else
	return -1;
    }
  else
    return -1;

  return 0;
}

static const char *whitespace = " \t\r\n";
#define TIME_FORMAT_STRING "%Y-%m-%dT%H:%M:%SL"

static int
parse_usersfile (const char *username,
		 const char *otp,
		 size_t window,
		 const char *passwd,
		 time_t * last_otp,
		 FILE * infh,
		 char **lineptr, size_t * n, uint64_t * new_moving_factor,
		 size_t * skipped_users)
{
  int bad_password = 0;

  *skipped_users = 0;

  while (getline (lineptr, n, infh) != -1)
    {
      char *saveptr;
      char *p = strtok_r (*lineptr, whitespace, &saveptr);
      unsigned digits, totpstepsize;
      char secret[32];
      size_t secret_length = sizeof (secret);
      uint64_t start_moving_factor = 0;
      int rc = 0;
      char *prev_otp = NULL;

      if (p == NULL)
	continue;

      /* Read token type */
      if (parse_type (p, &digits, &totpstepsize) != 0)
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
	  if (strcmp (p, "-") == 0)
	    {
	      if (*passwd != '\0')
		{
		  bad_password = 1;
		  rc = OATH_BAD_PASSWORD;
		}
	    }
	  else if (strcmp (p, "+") == 0)
	    {
	      /* Externally verified. */
	    }
	  else if (strcmp (p, passwd) != 0)
	    {
	      bad_password = 1;
	      rc = OATH_BAD_PASSWORD;
	    }
	  if (rc == OATH_BAD_PASSWORD)
	    {
	      (*skipped_users)++;
	      continue;
	    }
	  bad_password = 0;
	}

      /* Read key. */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p == NULL)
	continue;
      rc = oath_hex2bin (p, secret, &secret_length);
      if (rc != OATH_OK)
	return rc;

      /* Read (optional) moving factor. */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p && *p)
	{
	  char *endptr;
	  unsigned long long int ull = strtoull (p, &endptr, 10);
	  if (endptr && *endptr != '\0')
	    return OATH_INVALID_COUNTER;
	  start_moving_factor = ull;
	}

      /* Read (optional) last OTP */
      prev_otp = strtok_r (NULL, whitespace, &saveptr);

      /* Read (optional) last_otp */
      p = strtok_r (NULL, whitespace, &saveptr);
      if (p)
	{
	  struct tm tm;
	  char *ts;

	  ts = strptime (p, TIME_FORMAT_STRING, &tm);
	  if (ts == NULL || *ts != '\0')
	    return OATH_INVALID_TIMESTAMP;
	  tm.tm_isdst = -1;
	  if (last_otp)
	    {
	      *last_otp = mktime (&tm);
	      if (*last_otp == (time_t) - 1)
		return OATH_INVALID_TIMESTAMP;
	    }
	}

      if (prev_otp && strcmp (prev_otp, otp) == 0)
	return OATH_REPLAYED_OTP;

      if (totpstepsize == 0)
	rc = oath_hotp_validate (secret, secret_length,
				 start_moving_factor, window, otp);
      else if (prev_otp)
	{
	  int prev_otp_pos, this_otp_pos, tmprc;
	  rc = oath_totp_validate2 (secret, secret_length,
				    time (NULL), totpstepsize, 0, window,
				    &this_otp_pos, otp);
	  if (rc == OATH_INVALID_OTP)
	    {
	      (*skipped_users)++;
	      continue;
	    }
	  if (rc < 0)
	    return rc;
	  tmprc = oath_totp_validate2 (secret, secret_length,
				       time (NULL), totpstepsize, 0, window,
				       &prev_otp_pos, prev_otp);
	  if (tmprc >= 0 && prev_otp_pos >= this_otp_pos)
	    return OATH_REPLAYED_OTP;
	}
      else
	rc = oath_totp_validate (secret, secret_length,
				 time (NULL), totpstepsize, 0, window, otp);
      if (rc == OATH_INVALID_OTP)
	{
	  (*skipped_users)++;
	  continue;
	}
      if (rc < 0)
	return rc;
      *new_moving_factor = start_moving_factor + rc;
      return OATH_OK;
    }

  if (*skipped_users)
    {
      if (bad_password)
	return OATH_BAD_PASSWORD;
      else
	return OATH_INVALID_OTP;
    }

  return OATH_UNKNOWN_USER;
}

static int
update_usersfile2 (const char *username,
		   const char *otp,
		   FILE * infh,
		   FILE * outfh,
		   char **lineptr,
		   size_t * n, char *timestamp, uint64_t new_moving_factor,
		   size_t skipped_users)
{
  size_t got_users = 0;

  while (getline (lineptr, n, infh) != -1)
    {
      char *saveptr;
      char *origline;
      const char *user, *type, *passwd, *secret;
      int r;
      unsigned digits, totpstepsize;

      origline = strdup (*lineptr);

      type = strtok_r (*lineptr, whitespace, &saveptr);
      if (type == NULL)
	goto skip_line;

      /* Read token type */
      if (parse_type (type, &digits, &totpstepsize) != 0)
	goto skip_line;

      /* Read username */
      user = strtok_r (NULL, whitespace, &saveptr);
      if (user == NULL || strcmp (user, username) != 0
	  || got_users++ != skipped_users)
	goto skip_line;

      passwd = strtok_r (NULL, whitespace, &saveptr);
      if (passwd == NULL)
	passwd = "-";

      secret = strtok_r (NULL, whitespace, &saveptr);
      if (secret == NULL)
	secret = "-";

      r = fprintf (outfh, "%s\t%s\t%s\t%s\t%llu\t%s\t%s\n",
		   type, username, passwd, secret,
		   (unsigned long long) new_moving_factor, otp, timestamp);
      free (origline);
      if (r <= 0)
	return OATH_PRINTF_ERROR;
      continue;

    skip_line:
      r = fprintf (outfh, "%s", origline);
      free (origline);
      if (r <= 0)
	return OATH_PRINTF_ERROR;
      continue;
    }

  return OATH_OK;
}

static int
update_usersfile (const char *usersfile,
		  const char *username,
		  const char *otp,
		  FILE * infh,
		  char **lineptr,
		  size_t * n, char *timestamp, uint64_t new_moving_factor,
		  size_t skipped_users)
{
  FILE *outfh, *lockfh;
  int rc;
  char *newfilename, *lockfile;

  /* Rewind input file. */
  {
    int pos;

    pos = fseeko (infh, 0L, SEEK_SET);
    if (pos == -1)
      return OATH_FILE_SEEK_ERROR;
    clearerr (infh);
  }

  /* Open lockfile. */
  {
    int l;

    l = asprintf (&lockfile, "%s.lock", usersfile);
    if (lockfile == NULL || ((size_t) l) != strlen (usersfile) + 5)
      return OATH_PRINTF_ERROR;

    lockfh = fopen (lockfile, "w");
    if (!lockfh)
      {
	free (lockfile);
	return OATH_FILE_CREATE_ERROR;
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
	return OATH_FILE_LOCK_ERROR;
      }
  }

  /* Open the "new" file. */
  {
    int l;

    l = asprintf (&newfilename, "%s.new", usersfile);
    if (newfilename == NULL || ((size_t) l) != strlen (usersfile) + 4)
      {
	fclose (lockfh);
	free (lockfile);
	return OATH_PRINTF_ERROR;
      }

    outfh = fopen (newfilename, "w");
    if (!outfh)
      {
	free (newfilename);
	fclose (lockfh);
	free (lockfile);
	return OATH_FILE_CREATE_ERROR;
      }
  }

  /* Create the new usersfile content. */
  rc = update_usersfile2 (username, otp, infh, outfh, lineptr, n,
			  timestamp, new_moving_factor, skipped_users);

  /* On success, flush the buffers. */
  if (rc == OATH_OK && fflush (outfh) != 0)
    rc = OATH_FILE_FLUSH_ERROR;

  /* On success, sync the disks. */
  if (rc == OATH_OK && fsync (fileno (outfh)) != 0)
    rc = OATH_FILE_SYNC_ERROR;

  /* Close the file regardless of success. */
  if (fclose (outfh) != 0)
    rc = OATH_FILE_CLOSE_ERROR;

  /* On success, overwrite the usersfile with the new copy. */
  if (rc == OATH_OK && rename (newfilename, usersfile) != 0)
    rc = OATH_FILE_RENAME_ERROR;

  /* Something has failed, don't leave garbage lying around. */
  if (rc != OATH_OK)
    unlink (newfilename);

  free (newfilename);

  /* Complete, close the lockfile */
  if (fclose (lockfh) != 0)
    rc = OATH_FILE_CLOSE_ERROR;
  if (unlink (lockfile) != 0)
    rc = OATH_FILE_UNLINK_ERROR;
  free (lockfile);

  return rc;
}

/**
 * oath_authenticate_usersfile:
 * @usersfile: string with user credential filename, in UsersFile format
 * @username: string with name of user
 * @otp: string with one-time password to authenticate
 * @window: how many past/future OTPs to search
 * @passwd: string with password, or NULL to disable password checking
 * @last_otp: output variable holding last successful authentication
 *
 * Authenticate user named @username with the one-time password @otp
 * and (optional) password @passwd.  Credentials are read (and
 * updated) from a text file named @usersfile.
 *
 * Note that for TOTP the usersfile will only record the last OTP and
 * use that to make sure more recent OTPs have not been seen yet when
 * validating a new OTP.  That logics relies on using the same search
 * window for the same user.
 *
 * Returns: On successful validation, %OATH_OK is returned.  If the
 *   supplied @otp is the same as the last successfully authenticated
 *   one-time password, %OATH_REPLAYED_OTP is returned and the
 *   timestamp of the last authentication is returned in @last_otp.
 *   If the one-time password is not found in the indicated search
 *   window, %OATH_INVALID_OTP is returned.  Otherwise, an error code
 *   is returned.
 **/
int
oath_authenticate_usersfile (const char *usersfile,
			     const char *username,
			     const char *otp,
			     size_t window,
			     const char *passwd, time_t * last_otp)
{
  FILE *infh;
  char *line = NULL;
  size_t n = 0;
  uint64_t new_moving_factor;
  int rc;
  size_t skipped_users;

  infh = fopen (usersfile, "r");
  if (!infh)
    return OATH_NO_SUCH_FILE;

  rc = parse_usersfile (username, otp, window, passwd, last_otp,
			infh, &line, &n, &new_moving_factor, &skipped_users);

  if (rc == OATH_OK)
    {
      char timestamp[30];
      size_t max = sizeof (timestamp);
      struct tm now;
      time_t t;
      size_t l;
      mode_t old_umask;

      if (time (&t) == (time_t) - 1)
	return OATH_TIME_ERROR;

      if (localtime_r (&t, &now) == NULL)
	return OATH_TIME_ERROR;

      l = strftime (timestamp, max, TIME_FORMAT_STRING, &now);
      if (l != 20)
	return OATH_TIME_ERROR;

      old_umask = umask (~(S_IRUSR | S_IWUSR));

      rc = update_usersfile (usersfile, username, otp, infh,
			     &line, &n, timestamp, new_moving_factor,
			     skipped_users);

      umask (old_umask);
    }

  free (line);
  fclose (infh);

  return rc;
}
