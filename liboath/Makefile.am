# Copyright (C) 2009, 2010 Simon Josefsson

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

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

SUBDIRS = lib . tests gtk-doc

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -DLIBOATH_BUILDING
AM_CPPFLAGS += -I$(srcdir)/lib -I$(builddir)/lib

AM_CFLAGS = $(CFLAG_VISIBILITY)

lib_LTLIBRARIES = liboath.la

liboath_la_SOURCES = liboath.map oath.h global.c usersfile.c hotp.c
liboath_la_LIBADD = lib/libgnu.la
liboath_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -no-undefined

if HAVE_LD_VERSION_SCRIPT
liboath_la_LDFLAGS += -Wl,--version-script=$(srcdir)/liboath.map
else
liboath_la_LDFLAGS += -export-symbols-regex '^oath_.*'
endif
