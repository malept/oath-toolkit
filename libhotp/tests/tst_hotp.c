/*
 * tst_hotp.c - self-tests for libhotp
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

#include <error.h>
#include <stdlib.h>

int main (void)
{
  hotp_rc rc;

  rc = hotp_generate_otp (NULL, 0, 0, false, 0, NULL);
  if (rc != HOTP_OK)
    error (EXIT_FAILURE, 0, "hotp_generate_otp: %d", rc);

  return EXIT_SUCCESS;
}
