# Copyright (C) 2009-2012 Simon Josefsson

# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

AM_CPPFLAGS = -I$(srcdir)/..
AM_LDFLAGS = -no-install

LDADD = ../liboath.la

EXTRA_DIST = users.oath expect.oath

ctests = \
	tst_basic \
	tst_coding \
	tst_errors \
	tst_hotp_algo \
	tst_hotp_validate \
	tst_totp_algo \
	tst_totp_validate

check_PROGRAMS = $(ctests) tst_usersfile
dist_check_SCRIPTS = tst_usersfile.sh

TESTS = $(ctests) $(dist_check_SCRIPTS)

TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT)

AUTOMAKE_OPTIONS = parallel-tests
TEST_EXTENSIONS = .sh
LOG_COMPILER = $(VALGRIND)
