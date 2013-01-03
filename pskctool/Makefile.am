# Copyright (C) 2012-2013 Simon Josefsson

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = gl . tests

ACLOCAL_AMFLAGS = -I gl/m4

AM_CPPFLAGS = -I$(builddir)/../libpskc/include -I$(srcdir)/../libpskc/include
AM_CPPFLAGS += -I$(srcdir)/gl -I$(builddir)/gl

bin_PROGRAMS = pskctool
pskctool_SOURCES = pskctool.c
pskctool_LDADD = ../libpskc/libpskc.la gl/libgnu.la libpskctool_cmd.la

noinst_LTLIBRARIES = libpskctool_cmd.la
BUILT_SOURCES = pskctool_cmd.c pskctool_cmd.h
libpskctool_cmd_la_SOURCES = $(BUILT_SOURCES) pskctool.ggo
libpskctool_cmd_la_LIBADD = gl/libgnu.la
libpskctool_cmd_la_CFLAGS =

pskctool_cmd.h pskctool_cmd.c: pskctool.ggo Makefile.am
	$(AM_V_GEN)gengetopt --unamed-opts="FILE" \
		--no-handle-version --no-handle-help \
		--input $< --file-name pskctool_cmd

dist_man_MANS = pskctool.1
MAINTAINERCLEANFILES = $(dist_man_MANS)
EXTRA_DIST = pskctool.h2m

pskctool.1: $(srcdir)/pskctool.c $(srcdir)/pskctool.ggo $(srcdir)/pskctool.h2m $(srcdir)/configure.ac
	$(AM_V_GEN)$(HELP2MAN) \
		--include $(srcdir)/pskctool.h2m \
		--no-info \
		--output=$@ $(top_builddir)/pskctool$(EXEEXT)
