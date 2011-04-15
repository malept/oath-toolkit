# Copyright (C) 2009, 2010, 2011 Simon Josefsson

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

SUBDIRS = lib . tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)/../liboath -I$(srcdir)/lib -I$(builddir)/lib

bin_PROGRAMS = oathtool
oathtool_SOURCES = oathtool.c
oathtool_LDADD = ../liboath/liboath.la lib/libgnu.la liboathtool_cmd.la

noinst_LTLIBRARIES = liboathtool_cmd.la
BUILT_SOURCES = oathtool_cmd.c oathtool_cmd.h
liboathtool_cmd_la_SOURCES = $(BUILT_SOURCES) oathtool.ggo
liboathtool_cmd_la_LIBADD = lib/libgnu.la
liboathtool_cmd_la_CFLAGS =

$(BUILT_SOURCES): oathtool.ggo Makefile.am
	gengetopt --unamed-opts="KEY [OTP]" \
		--no-handle-version --no-handle-help \
		--input $< --file-name oathtool_cmd

dist_man_MANS = oathtool.1
MAINTAINERCLEANFILES = $(dist_man_MANS)
EXTRA_DIST = oathtool.h2m

oathtool.1: $(srcdir)/oathtool.c $(srcdir)/oathtool.ggo $(srcdir)/oathtool.h2m $(srcdir)/configure.ac
	$(HELP2MAN) \
		--include $(srcdir)/oathtool.h2m \
		--no-info \
		--output=$@ $(top_builddir)/oathtool$(EXEEXT)
