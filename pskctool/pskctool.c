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

/* Gnulib. */
#include "progname.h"
#include "error.h"
#include "version-etc.h"

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
check (void)
{
  puts ("check");
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

  rc = pskc_init ();
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "libpskc initialization failed: %s",
	   pskc_strerror (rc));

  if (args_info.check_flag)
    check ();
  else
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
      return EXIT_SUCCESS;
    }

  pskc_done ();

  return EXIT_SUCCESS;
}
