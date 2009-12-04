# Copyright (C) 2009  Simon Josefsson

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

SUBDIRS = lib . tests gtk-doc

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -DLIBHOTP_BUILDING
AM_CPPFLAGS += -I$(srcdir)/lib -I$(builddir)/lib

AM_CFLAGS = $(CFLAG_VISIBILITY)

lib_LTLIBRARIES = libhotp.la

libhotp_la_SOURCES = libhotp.map hotp.c hotp.h
libhotp_la_LIBADD = lib/libgnu.la
libhotp_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -no-undefined

if HAVE_LD_VERSION_SCRIPT
libhotp_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libhotp.map
else
libhotp_la_LDFLAGS += -export-symbols-regex '^hotp_.*'
endif
