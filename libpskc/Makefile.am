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

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

SUBDIRS = gl schemas include . examples tests gtk-doc man

ACLOCAL_AMFLAGS = -I m4 -I gl/m4

AM_CPPFLAGS = -DPSKC_BUILDING
AM_CPPFLAGS += -I$(srcdir)/gl -I$(builddir)/gl
AM_CPPFLAGS += -I$(srcdir)/include -I$(builddir)/include
AM_CPPFLAGS += $(XML_CPPFLAGS) $(XMLSEC_CFLAGS)
AM_CPPFLAGS += -DPSKC_SCHEMA_CATALOG=\"$(pskcschemadir)/catalog-pskc.xml\"

AM_CFLAGS = $(WARN_CFLAGS) $(WERROR_CFLAGS)
AM_CFLAGS += $(CFLAG_VISIBILITY)

lib_LTLIBRARIES = libpskc.la

libpskc_la_SOURCES = internal.h libpskc.map
libpskc_la_SOURCES += global.c errors.c enums.c
libpskc_la_SOURCES += container.c parser.c validate.c build.c sign.c output.c
libpskc_la_LIBADD = gl/libgnu.la
libpskc_la_LDFLAGS = $(XML_LIBS) $(XMLSEC_LIBS) \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -no-undefined

if HAVE_LD_VERSION_SCRIPT
libpskc_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libpskc.map
else
libpskc_la_LDFLAGS += -export-symbols-regex '^pskc_.*'
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libpskc.pc
