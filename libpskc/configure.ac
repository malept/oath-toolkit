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

AC_INIT([libpskc],
  m4_esyscmd([../build-aux/oath-toolkit-version ../.tarball-version]),
  [oath-toolkit-help@nongnu.org],,
  [http://www.nongnu.org/oath-toolkit/])

VERSION_NUMBER=`../build-aux/oath-toolkit-version ../.tarball-version vn`
AC_SUBST(VERSION_NUMBER)

# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 0)
AC_SUBST(LT_REVISION, 1)
AC_SUBST(LT_AGE, 0)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])

AC_LIBTOOL_WIN32_DLL
AC_PROG_CC
gl_EARLY
AC_PROG_LIBTOOL
gl_INIT
GTK_DOC_CHECK(1.1)

AM_PATH_XML2([], [], [AC_MSG_ERROR(["cannot find libxml2"])])
PKG_CHECK_MODULES(XMLSEC, xmlsec1 >= 1.2.16, [xmlsec=yes], [xmlsec=no])
if test "$xmlsec" != "no" ; then
  AC_DEFINE(USE_XMLSEC, 1, [Define to 1 if you want to use xmlsec.])
fi
AC_MSG_CHECKING([if xmlsec should be used])
AC_MSG_RESULT($xmlsec)
AM_CONDITIONAL(XMLSEC, test x$xmlsec = xyes)

AC_ARG_ENABLE([gcc-warnings],
  [AS_HELP_STRING([--enable-gcc-warnings],
                  [turn on lots of GCC warnings (for developers)])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for gcc-warnings option]) ;;
   esac
   gl_gcc_warnings=$enableval],
  [gl_gcc_warnings=no]
)

if test "$gl_gcc_warnings" = yes; then
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
  AC_SUBST([WERROR_CFLAGS])
  gl_MANYWARN_ALL_GCC([warnings])

  # Set up the list of the pointless, undesired warnings.
  nw=
  nw="$nw -Wsystem-headers"       # Don't let system headers trigger warnings
  nw="$nw -Wundef"                # All compiler preprocessors support #if UNDEF
  nw="$nw -Wtraditional"          # All compilers nowadays support ANSI C
  nw="$nw -Wconversion"           # These warnings usually don't point to mistakes.
  nw="$nw -Wsign-conversion"      # Likewise.
  nw="$nw -Wtraditional-conversion" # Too many warnings for now
  nw="$nw -Wlogical-op"           # any use of strchr provokes this
  nw="$nw -Wunreachable-code"     # any use of strchr provokes this
  nw="$nw -Wc++-compat"           # We don't care about C++ compilers
  nw="$nw -Wcast-qual"            # Triggers oath_hotp_validate, how to fix?
  nw="$nw -Wpadded"               # Padding internal structs is useless.
  nw="$nw -Wsuggest-attribute=pure" # Is it worth using attributes?
  nw="$nw -Wsuggest-attribute=const" # Is it worth using attributes?

  # Enable all GCC warnings not in this list.
  gl_MANYWARN_COMPLEMENT([warnings], [$warnings], [$nw])
  for w in $warnings; do
    gl_WARN_ADD([$w])
  done
  gl_WARN_ADD([-fdiagnostics-show-option])
  gl_WARN_ADD([-Wno-error=overlength-strings])
  AC_SUBST([WARN_CFLAGS])

  AC_DEFINE([_FORTIFY_SOURCE], [2],
    [enable compile-time and run-time bounds-checking, and some warnings])
  AC_DEFINE([GNULIB_POSIXCHECK], [1], [enable some gnulib portability checks])
fi

AC_CONFIG_FILES([
  Makefile
  libpskc.pc
  include/Makefile
  include/pskc/Makefile
  include/pskc/version.h
  gl/Makefile
  gl/tests/Makefile
  examples/Makefile
  tests/Makefile
  gtk-doc/Makefile
  gtk-doc/version.xml
  man/Makefile
])
AC_OUTPUT
