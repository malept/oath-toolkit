# Copyright (C) 2009-2013 Simon Josefsson

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

SUBDIRS = . tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(builddir)/../liboath

EXTRA_DIST = README

pammoddir = $(PAMDIR)
pammod_LTLIBRARIES = pam_oath.la

pam_oath_la_SOURCES = pam_oath.c
# XXX add -Wl,-x too?  PAM documentation suggests it.
pam_oath_la_LIBADD = ../liboath/liboath.la
pam_oath_la_LDFLAGS = -module -avoid-version
