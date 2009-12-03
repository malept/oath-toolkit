AC_INIT([hotptool], [1.0.0], [simon@josefsson.org], [hotptool],
        [http://josefsson.org/hotp-toolkit/])
AC_COPYRIGHT([Copyright (C) 2009 Simon Josefsson])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)

AC_LIBTOOL_WIN32_DLL
AC_PROG_CC
gl_EARLY
AC_PROG_LIBTOOL
gl_INIT

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  tests/Makefile
])
AC_OUTPUT
