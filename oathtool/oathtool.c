/*
 * oathtool.c - command line tool for OATH one-time passwords
 * Copyright (C) 2009, 2010 Simon Josefsson
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include "oath.h"

#include <stdlib.h>
#include <string.h>

/* Gnulib. */
#include "progname.h"
#include "error.h"
#include "version-etc.h"

#include "oathtool_cmd.h"

const char version_etc_copyright[] =
  /* Do *not* mark this string for translation.  %s is a copyright
     symbol suitable for this locale, and %d is the copyright
     year.  */
  "Copyright %s %d Simon Josefsson.";

/* This feature is available in gcc versions 2.5 and later.  */
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5)
# define OATH_ATTR_NO_RETRUN
#else
# define OATH_ATTR_NO_RETRUN __attribute__ ((__noreturn__))
#endif

static void
usage (int status) OATH_ATTR_NO_RETRUN;

static void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s --help' for more information.\n",
	     program_name);
  else
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
    }
  exit (status);
}

#define generate_otp_p(n) ((n) == 1)
#define validate_otp_p(n) ((n) == 2)

#define EXIT_OTP_INVALID 2

int
main (int argc, char *argv[])
{
  struct gengetopt_args_info args_info;
  char *secret;
  size_t secretlen;
  int rc;
  size_t window;
  uint64_t moving_factor;
  unsigned digits;
  char otp[10];

  set_program_name (argv[0]);

  if (cmdline_parser (argc, argv, &args_info) != 0)
    return EXIT_FAILURE;

  if (args_info.version_given)
    {
      const char *p = PACKAGE_NAME;
      if (strcmp (oath_check_version (NULL), PACKAGE_VERSION) != 0)
	p = PACKAGE_STRING;
      version_etc (stdout, "oathtool", p, oath_check_version (NULL),
		   "Simon Josefsson", (char *) NULL);
      return EXIT_SUCCESS;
    }

  if (args_info.help_given)
    usage (EXIT_SUCCESS);

  if (args_info.inputs_num == 0)
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
      return EXIT_SUCCESS;
    }

  secretlen = 1 + strlen (args_info.inputs[0]) / 2;
  secret = malloc (secretlen);

  rc = oath_hex2bin (args_info.inputs[0], secret, &secretlen);
  if (rc != OATH_OK)
    error (EXIT_FAILURE, 0, "hex decoding of secret key failed");

  if (args_info.counter_orig)
    moving_factor = args_info.counter_arg;
  else
    moving_factor = 0;

  if (args_info.digits_orig)
    digits = args_info.digits_arg;
  else
    digits = 6;

  if (args_info.window_orig)
    window = args_info.window_arg;
  else
    window = 0;

  if (digits != 6 && digits != 7 && digits != 8)
    error (EXIT_FAILURE, 0, "only digits 6, 7 and 8 are supported");

  if (validate_otp_p (args_info.inputs_num) && !args_info.digits_orig)
    digits = strlen (args_info.inputs[1]);
  else if (validate_otp_p (args_info.inputs_num) && args_info.digits_orig &&
	   args_info.digits_arg != strlen (args_info.inputs[1]))
    error (EXIT_FAILURE, 0, "given one-time password has bad length %d != %d",
	   args_info.digits_arg, strlen (args_info.inputs[1]));

  if (generate_otp_p (args_info.inputs_num))
    {
      size_t iter = 0;

      do
	{
	  rc = oath_hotp_generate (secret,
				   secretlen,
				   moving_factor + iter,
				   digits,
				   false,
				   OATH_HOTP_DYNAMIC_TRUNCATION,
				   otp);
	  if (rc != OATH_OK)
	    error (EXIT_FAILURE, 0,
		   "generating one-time password failed (%d)", rc);

	  printf ("%s\n", otp);
	}
      while (window - iter++ > 0);
    }
  else if (validate_otp_p (args_info.inputs_num))
    {
      rc = oath_hotp_validate (secret,
			       secretlen,
			       moving_factor,
			       window,
			       args_info.inputs[1]);
      if (rc == OATH_INVALID_OTP)
	error (EXIT_OTP_INVALID, 0,
	       "password \"%s\" not found in range %ld .. %ld",
	       args_info.inputs[1],
	       (long) moving_factor, (long) moving_factor + window);
      else if (rc < 0)
	error (EXIT_FAILURE, 0,
	       "validating one-time password failed (%d)", rc);
      printf ("%d\n", rc);
    }

  return EXIT_SUCCESS;
}
