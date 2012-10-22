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

noinst_PROGRAMS = serialno pskc2csv

EXTRA_DIST = pskc-mini.xml pskc-hotp.xml pskctool-h.txt pskc-hotp-human.txt

pskctool-h.txt: $(top_srcdir)/../pskctool/pskctool.ggo $(top_srcdir)/../pskctool/pskctool.c
	$(top_builddir)/../pskctool/pskctool > tmp
	sed '1,2d' < tmp > $@
