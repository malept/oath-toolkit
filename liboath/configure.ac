# Copyright (C) 2009, 2010, 2011 Simon Josefsson

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

AC_INIT([liboath],
	m4_esyscmd([../build-aux/git-version-gen .tarball-version \
		's/[a-z-]*-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)/v\1.\2.\3/']),
	[oath-toolkit-help@nongnu.org],,
	[http://www.nongnu.org/oath-toolkit/])

# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 1)
AC_SUBST(LT_REVISION, 0)
AC_SUBST(LT_AGE, 1)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_LIBTOOL_WIN32_DLL
AC_PROG_CC
gl_EARLY
AC_PROG_LIBTOOL
gl_INIT
GTK_DOC_CHECK(1.1)

AC_CONFIG_FILES([
  Makefile
  liboath.pc
  lib/Makefile
  tests/Makefile
  gtk-doc/Makefile
])
AC_OUTPUT
