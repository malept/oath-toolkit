# Copyright (C) 2009, 2010 Simon Josefsson

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

AM_CPPFLAGS = -I$(srcdir)/../libhotp -I$(srcdir)/lib -I$(builddir)/lib

bin_PROGRAMS = hotptool
hotptool_SOURCES = hotptool.c
hotptool_LDADD = ../libhotp/libhotp.la lib/libgnu.la libhotptool_cmd.la

noinst_LTLIBRARIES = libhotptool_cmd.la
BUILT_SOURCES = hotptool_cmd.c hotptool_cmd.h
libhotptool_cmd_la_SOURCES = $(BUILT_SOURCES) hotptool.ggo
libhotptool_cmd_la_LIBADD = lib/libgnu.la
libhotptool_cmd_la_CFLAGS =

$(BUILT_SOURCES): hotptool.ggo Makefile.am
	gengetopt --unamed-opts="KEY [OTP]" \
		--no-handle-version --no-handle-help \
		--input $< --file-name hotptool_cmd

dist_man_MANS = hotptool.1
MAINTAINERCLEANFILES = $(dist_man_MANS)
EXTRA_DIST = hotptool.h2m

hotptool.1: $(srcdir)/hotptool.c $(srcdir)/hotptool.ggo $(srcdir)/hotptool.h2m $(srcdir)/configure.ac
	$(HELP2MAN) \
		--include hotptool.h2m \
		--output=$@ $(top_builddir)/hotptool$(EXEEXT)
