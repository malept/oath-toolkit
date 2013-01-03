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

AC_INIT([oath-toolkit],
  m4_esyscmd([build-aux/oath-toolkit-version .tarball-version]),
  [oath-toolkit-help@nongnu.org],,
  [http://www.nongnu.org/oath-toolkit/])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror])
AM_SILENT_RULES([yes])

AC_CHECK_HEADERS_ONCE([security/pam_appl.h])
AC_ARG_ENABLE([pam],
  [AS_HELP_STRING([--disable-pam], [disable PAM module])],
   [],
   [enable_pam=$ac_cv_header_security_pam_appl_h])
AM_CONDITIONAL([ENABLE_PAM], [test "x$enable_pam" = xyes])

AC_ARG_ENABLE([pskc],
  [AS_HELP_STRING([--disable-pskc], [disable PSKC support])],
   [],
   [enable_pskc=yes])
if test "x$enable_pskc" = xyes; then
  AM_PATH_XML2([], [], [enable_pskc=no])
fi
AM_CONDITIONAL([ENABLE_PSKC], [test "x$enable_pskc" = xyes])

gl_EARLY
gl_INIT

AC_CONFIG_SUBDIRS([
  liboath
  oathtool
])
if test "x$enable_pskc" = xyes; then
  AC_CONFIG_SUBDIRS([libpskc])
  AC_CONFIG_SUBDIRS([pskctool])
fi
if test "x$enable_pam" = xyes; then
  AC_CONFIG_SUBDIRS([pam_oath])
fi
AC_CONFIG_FILES([
  Makefile
  gl/Makefile
  gl/tests/Makefile
])
AC_OUTPUT

AC_MSG_NOTICE([summary of build options:

  version:          ${VERSION}
  Install prefix:   ${prefix}
  Compiler:         ${CC}
  Build PAM module: $enable_pam
  PSKC:             $enable_pskc
])
