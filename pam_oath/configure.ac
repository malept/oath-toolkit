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

AC_INIT([pam_oath],
  m4_esyscmd([../build-aux/git-version-gen .tarball-version \
    's/[a-z-]*-\([0-9]*\)-\([0-9]*\)-\([0-9]*\)\(-\([0-9]*\)\)\?.*/v\1.\2.\3.\5/;s/\.$//']),
  [oath-toolkit-help@nongnu.org],,
  [http://www.nongnu.org/oath-toolkit/])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_LIBTOOL_WIN32_DLL
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_CHECK_HEADERS([security/pam_appl.h], [],
  [AC_MSG_ERROR([[PAM header files not found, install libpam-dev.]])])
AC_CHECK_HEADERS([security/pam_modules.h security/_pam_macros.h], [], [],
  [#include <security/pam_appl.h>])

AC_CHECK_LIB(pam, pam_get_item)

AC_SUBST(PAMDIR, "\$(exec_prefix)/lib/security")
AC_ARG_WITH(pam-dir,
  AC_HELP_STRING([--with-pam-dir=DIR],
                 [Where to install PAM module [[PREFIX/lib/security]]]),
  [case "${withval}" in
     /*) PAMDIR="${withval}";;
     ./*|../*) AC_MSG_ERROR(Bad value for --with-pam-dir);;
     *)  PAMDIR="\$(exec_prefix)/lib/${withval}";;
   esac])
AC_MSG_NOTICE([PAM installation path $PAMDIR])

AC_ARG_ENABLE(root-tests,
  AC_HELP_STRING([--enable-root-tests],
                 [enable some self-tests that need root permissions]),
  root_tests=yes, root_tests=$enableval)
AM_CONDITIONAL([ROOT_TESTS], [test x$root_tests = xyes])

AC_CONFIG_FILES([
  Makefile
  tests/Makefile
])
AC_OUTPUT
