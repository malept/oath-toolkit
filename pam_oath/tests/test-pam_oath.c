/*
 * test-pam_oath.c - self-tests for PAM module for OATH one-time passwords
 * Copyright (C) 2009-2013 Simon Josefsson
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

#include <stdio.h>
#include <stdlib.h>

/* pam_sm_authenticate */
#define PAM_SM_AUTH
#include <security/pam_modules.h>

int
main (int argc, char **argv)
{
  pam_handle_t *pamh = NULL;
  int rc;

  rc = pam_sm_authenticate (pamh, 0, 1, (const char **) argv);

  printf ("rc %d: %s\n", rc, pam_strerror (pamh, rc));

  return 0;
}
