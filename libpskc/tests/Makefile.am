# Copyright (C) 2012 Simon Josefsson

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

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
AM_LDFLAGS = -no-install

LDADD = ../libpskc.la

EXTRA_DIST = libpskc.supp

ctests = \
	tst_basic \
	tst_errors \
	tst_accessors \
	tst_validate

check_PROGRAMS = $(ctests)

TESTS = $(ctests)

TESTS_ENVIRONMENT = EXEEXT=$(EXEEXT)

AUTOMAKE_OPTIONS = parallel-tests
TEST_EXTENSIONS = .sh
LOG_COMPILER = $(VALGRIND)

# Schema validation needs the XML Schemas, use our non-installed catalog.
TESTS_ENVIRONMENT += XML_CATALOG_FILES=$(top_builddir)/schemas/catalog-pskc.xml
