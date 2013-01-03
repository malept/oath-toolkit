/*
 * pskctool.c - Manage Portable Symmetric Key Container (PSKC) data.
 * Copyright (C) 2012-2013 Simon Josefsson
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

#include <pskc/pskc.h>

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
version (void)
  PSKC_ATTR_NO_RETURN;
/* *INDENT-ON* */

static void
version (void)
{
  char *p;
  int l = -1;

  if (strcmp (pskc_check_version (NULL), PSKC_VERSION) != 0)
    l = asprintf (&p, "PSKC Toolkit libpskc.so %s pskc.h %s",
		  pskc_check_version (NULL), PSKC_VERSION);
  else if (strcmp (PSKC_VERSION, PACKAGE_VERSION) != 0)
    l = asprintf (&p, "PSKC Toolkit %s", pskc_check_version (NULL));
  version_etc (stdout, "pskctool", l == -1 ? "PSKC Toolkit" : p,
	       PACKAGE_VERSION, "Simon Josefsson", (char *) NULL);
  if (l != -1)
    free (p);

  exit (EXIT_SUCCESS);
}

static void
debuglog (const char *msg)
{
  fprintf (stderr, "debug: %s\n", msg);
}

static pskc_t *
get_container (const struct gengetopt_args_info *args_info)
{
  const char *filename = args_info->inputs ? args_info->inputs[0] : NULL;
  int strict = args_info->strict_flag;
  pskc_t *container;
  char *buffer;
  size_t len;
  int rc;

  rc = pskc_init (&container);
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "initializing PSKC structure: %s",
	   pskc_strerror (rc));

  if (filename)
    buffer = read_binary_file (filename, &len);
  else
    buffer = fread_file (stdin, &len);
  if (buffer == NULL)
    error (EXIT_FAILURE, errno, "read");

  rc = pskc_parse_from_memory (container, len, buffer);
  if (!strict && rc == PSKC_PARSE_ERROR)
    fprintf (stderr, "warning: parse error (use -d to diagnose), output "
	     "may be incomplete\n");
  else if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "parsing PSKC data: %s", pskc_strerror (rc));

  free (buffer);

  return container;
}

static void
validate (const struct gengetopt_args_info *args_info)
{
  int quiet = args_info->quiet_flag;
  pskc_t *container = get_container (args_info);
  int rc;
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

  pskc_done (container);
}

static void
verify (const struct gengetopt_args_info *args_info)
{
  const char *verify_crt =
    args_info->verify_crt_given ? args_info->verify_crt_arg : NULL;
  int quiet = args_info->quiet_flag;
  pskc_t *container = get_container (args_info);
  int rc;
  int valid_signature;

  rc = pskc_verify_x509crt (container, verify_crt, &valid_signature);
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "verifying PSKC data: %s", pskc_strerror (rc));

  if (quiet && !valid_signature)
    error (EXIT_FAILURE, 0, "");
  if (!quiet && valid_signature)
    puts ("OK");
  else if (!quiet)
    puts ("FAIL");

  pskc_done (container);
}

static void
sign (const struct gengetopt_args_info *args_info)
{
  const char *sign_key =
    args_info->sign_key_given ? args_info->sign_key_arg : NULL;
  const char *sign_crt =
    args_info->sign_crt_given ? args_info->sign_crt_arg : NULL;
  pskc_t *container = get_container (args_info);
  char *out;
  size_t len;
  int rc;

  rc = pskc_sign_x509 (container, sign_key, sign_crt);
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "signing PSKC data: %s", pskc_strerror (rc));

  rc = pskc_output (container, PSKC_OUTPUT_XML, &out, &len);
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "converting PSKC data: %s", pskc_strerror (rc));

  printf ("%.*s", (int) len, out);

  pskc_free (out);

  pskc_done (container);
}

static void
info (const struct gengetopt_args_info *args_info)
{
  int verbose = args_info->verbose_flag;
  int quiet = args_info->quiet_flag;
  pskc_t *container = get_container (args_info);
  char *out;
  size_t len;
  int rc;

  if (!quiet)
    {
      rc = pskc_output (container, PSKC_OUTPUT_HUMAN_COMPLETE, &out, &len);
      if (rc != PSKC_OK)
	error (EXIT_FAILURE, 0, "converting PSKC data: %s",
	       pskc_strerror (rc));

      printf ("%.*s", (int) len, out);

      pskc_free (out);
    }

  if (!quiet && verbose)
    printf ("\n");

  if (verbose)
    {
      rc = pskc_output (container, PSKC_OUTPUT_INDENTED_XML, &out, &len);
      if (rc != PSKC_OK)
	error (EXIT_FAILURE, 0, "converting PSKC data: %s",
	       pskc_strerror (rc));

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

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    error (EXIT_FAILURE, 0, "libpskc initialization failed: %s",
	   pskc_strerror (rc));

  if (args_info.debug_flag)
    pskc_global_log (debuglog);

  if (args_info.version_given)
    version ();
  else if (args_info.validate_flag)
    validate (&args_info);
  else if (args_info.sign_flag)
    sign (&args_info);
  else if (args_info.verify_flag)
    verify (&args_info);
  else if (args_info.info_flag)
    info (&args_info);
  else
    {
      cmdline_parser_print_help ();
      emit_bug_reporting_address ();
    }

  pskc_global_done ();

  return EXIT_SUCCESS;
}
