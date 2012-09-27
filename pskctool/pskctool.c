/*
 * pskctool.c - Manage Portable Symmetric Key Container (PSKC) data.
 * Copyright (C) 2012 Simon Josefsson
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

#include "pskc.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Gnulib. */
#include "progname.h"
#include "error.h"
#include "version-etc.h"
#include "read-file.h"

#include "pskctool_cmd.h"

const char version_etc_copyright[] =
  /* Do *not* mark this string for translation.  %s is a copyright
     symbol suitable for this locale, and %d is the copyright
     year.  */
  "Copyright %s %d Simon Josefsson.";

/* This feature is available in gcc versions 2.5 and later.  */
#if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5)
#define PSKC_ATTR_NO_RETURN
#else
#define PSKC_ATTR_NO_RETURN __attribute__ ((__noreturn__))
#endif

/* *INDENT-OFF* */
static void
usage (int status)
  PSKC_ATTR_NO_RETURN;
/* *INDENT-ON* */

static void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s --help' for more information.\n", program_name);
  else
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
    }
  exit (status);
}

static void
checkvalidate (const char *filename, int validate, int quiet)
{
  char *buffer;
  size_t len;
  pskc *container;
  int rc;

  if (filename)
    buffer = read_binary_file (filename, &len);
  else
    buffer = fread_file (stdin, &len);
  if (buffer == NULL)
    error (EXIT_FAILURE, errno, "read");

  rc = pskc_init_from_memory (&container, len, buffer);
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "parsing PSKC data: %s", pskc_strerror (rc));

  free (buffer);

  if (validate)
    {
      int isvalid;

      rc = pskc_validate (container, &isvalid);
      if (rc != PSKC_OK)
	error (EXIT_FAILURE, 0, "validation of PSKC data failed: %s",
	       pskc_strerror (rc));

      if (quiet && !isvalid)
	error (EXIT_FAILURE, 0, "");
      if (!quiet && isvalid)
	puts ("OK");
      else if (!quiet)
	puts ("FAIL");
    }
  else if (!quiet)
    {
      char *out;

      rc = pskc_output (container, PSKC_OUTPUT_HUMAN_COMPLETE, &out, &len);
      if (rc != PSKC_OK)
	error (EXIT_FAILURE, 0, "printing PSKC data: %s", pskc_strerror (rc));

      printf ("%.*s", (int) len, out);

      pskc_free (out);
    }

  pskc_done (container);
}

int
main (int argc, char *argv[])
{
  struct gengetopt_args_info args_info;
  int rc;

  set_program_name (argv[0]);

  if (cmdline_parser (argc, argv, &args_info) != 0)
    return EXIT_FAILURE;

  if (args_info.version_given)
    {
      char *p;
      int l = -1;

      if (strcmp (pskc_check_version (NULL), PSKC_VERSION) != 0)
	l = asprintf (&p, "PSKC Toolkit libpskc.so %s pskc.h %s",
		      pskc_check_version (NULL), PSKC_VERSION);
      else if (strcmp (PSKC_VERSION, PACKAGE_VERSION) != 0)
	l = asprintf (&p, "PSKC Toolkit %s",
		      pskc_check_version (NULL), PSKC_VERSION);
      version_etc (stdout, "pskctool", l == -1 ? "PSKC Toolkit" : p,
		   PACKAGE_VERSION, "Simon Josefsson", (char *) NULL);
      if (l != -1)
	free (p);
      return EXIT_SUCCESS;
    }

  if (args_info.help_given)
    usage (EXIT_SUCCESS);

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "libpskc initialization failed: %s",
	   pskc_strerror (rc));

  if (args_info.check_flag || args_info.validate_flag)
    checkvalidate (args_info.inputs ? args_info.inputs[0] : NULL,
		   args_info.validate_flag, args_info.quiet_flag);
  else
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
      return EXIT_SUCCESS;
    }

  pskc_global_done ();

  return EXIT_SUCCESS;
}
