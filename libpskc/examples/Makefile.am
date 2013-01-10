# Copyright (C) 2012-2013 Simon Josefsson

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

noinst_PROGRAMS = serialno pskc2csv pskcsign pskcverify

# Remember to update EXAMPLES_SOURCE_FILES in ../gtk-doc/Makefile.am
# if you add examples.

# Manually maintained.
EXTRA_DIST = pskc-mini.xml pskc-hotp.xml

# Semi-generated.
EXTRA_DIST += pskctool-h.txt
EXTRA_DIST += pskc-hotp-human.txt pskc-hotp-signed.xml
EXTRA_DIST += pskc-figure6-debug.txt

pskc-hotp-human.txt: $(srcdir)/pskc-hotp.xml
	$(top_builddir)/../pskctool/pskctool --info \
		$(srcdir)/pskc-hotp.xml > tmp 2>&1
	mv tmp $@

pskc-hotp-signed.xml: $(srcdir)/pskc-hotp.xml
	$(top_builddir)/../pskctool/pskctool --sign \
		--sign-key $(top_srcdir)/../pskctool/tests/pskc-ee-key.pem \
		--sign-crt $(top_srcdir)/../pskctool/tests/pskc-ee-crt.pem \
		$(srcdir)/pskc-hotp.xml > tmp 2>&1
	mv tmp $@

pskctool-h.txt: $(top_srcdir)/../pskctool/pskctool.ggo $(top_srcdir)/../pskctool/pskctool.c
	$(top_builddir)/../pskctool/pskctool | sed '1,2d;s/ \+$$//' > tmp 2>&1
	mv tmp $@

pskc-figure6-debug.txt: $(top_srcdir)/../pskctool/tests/pskc-figure6.xml
	$(top_builddir)/../pskctool/pskctool --info --debug --quiet \
		$(top_srcdir)/../pskctool/tests/pskc-figure6.xml > tmp 2>&1
	mv tmp $@
