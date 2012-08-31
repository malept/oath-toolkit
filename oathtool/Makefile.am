# Copyright (C) 2009-2012 Simon Josefsson

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

AM_CPPFLAGS = -I$(builddir)/../liboath -I$(srcdir)/gl -I$(builddir)/gl

bin_PROGRAMS = oathtool
oathtool_SOURCES = oathtool.c
oathtool_LDADD = ../liboath/liboath.la gl/libgnu.la liboathtool_cmd.la

noinst_LTLIBRARIES = liboathtool_cmd.la
BUILT_SOURCES = oathtool_cmd.c oathtool_cmd.h
liboathtool_cmd_la_SOURCES = $(BUILT_SOURCES) oathtool.ggo
liboathtool_cmd_la_LIBADD = gl/libgnu.la
liboathtool_cmd_la_CFLAGS =

oathtool_cmd.h oathtool_cmd.c: oathtool.ggo Makefile.am
	gengetopt --unamed-opts="KEY [OTP]" \
		--no-handle-version --no-handle-help \
		--input $< --file-name oathtool_cmd && \
	perl -pi -e 's/HAVE_LONG_LONG/HAVE_LONG_LONG_INT/' oathtool_cmd.h oathtool_cmd.c && \
	perl -pi -e 's/(long long int) strtol /(long long int) strtoll /' oathtool_cmd.c

dist_man_MANS = oathtool.1
MAINTAINERCLEANFILES = $(dist_man_MANS)
EXTRA_DIST = oathtool.h2m

oathtool.1: $(srcdir)/oathtool.c $(srcdir)/oathtool.ggo $(srcdir)/oathtool.h2m $(srcdir)/configure.ac
	$(HELP2MAN) \
		--include $(srcdir)/oathtool.h2m \
		--no-info \
		--output=$@ $(top_builddir)/oathtool$(EXEEXT)
