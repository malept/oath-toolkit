# Copyright (C) 2009  Simon Josefsson

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

AC_INIT([hotp-toolkit], [1.0.0], [simon@josefsson.org])
AC_COPYRIGHT([Copyright (C) 2009 Simon Josefsson])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])

gl_EARLY
gl_INIT

AC_CONFIG_SUBDIRS([
  libhotp
  hotptool
  pam_hotp
])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
