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

SUBDIRS = . tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I. -I$(srcdir)/../libhotp

EXTRA_DIST = README

libdir = $(PAMDIR)

lib_LTLIBRARIES = pam_hotp.la

pam_hotp_la_SOURCES = pam_hotp.c
# XXX add -Wl,-x too?  PAM documentation suggests it.
pam_hotp_la_LIBADD = ../libhotp/libhotp.la
pam_hotp_la_LDFLAGS = -module -avoid-version
