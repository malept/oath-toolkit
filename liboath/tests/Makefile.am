# Copyright (C) 2009-2011 Simon Josefsson

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
AM_LDFLAGS = -no-install -static

LDADD = ../liboath.la

EXTRA_DIST = users.hotp expect.hotp

TESTS = \
	tst_basic$(EXEEXT) \
	tst_errors$(EXEEXT) \
	tst_hotp_algo$(EXEEXT) \
	tst_hotp_validate$(EXEEXT) \
	tst_hotp_usersfile.sh \
	tst_totp_algo$(EXEEXT) \
	tst_totp_validate$(EXEEXT)
check_PROGRAMS = \
	tst_basic$(EXEEXT) \
	tst_errors$(EXEEXT) \
	tst_hotp_algo$(EXEEXT) \
	tst_hotp_validate$(EXEEXT) \
	tst_hotp_usersfile$(EXEEXT) \
	tst_totp_algo$(EXEEXT) \
	tst_totp_validate$(EXEEXT)
dist_check_SCRIPTS = tst_hotp_usersfile.sh

TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT)

AUTOMAKE_OPTIONS = parallel-tests
TEST_EXTENSIONS = .sh
LOG_COMPILER = $(VALGRIND)
