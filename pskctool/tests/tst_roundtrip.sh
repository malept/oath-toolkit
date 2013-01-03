#!/bin/sh

# tst_roundtrip.sh - test that pskctool generates what it parses without errors
# Copyright (C) 2012-2013 Simon Josefsson

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

set -e

PSKCTOOL=../pskctool

srcdir="${srcdir:-.}"
pskc_all="$srcdir/pskc-all.xml"
pskc_all_human="$srcdir/pskc-all-human.txt"

$PSKCTOOL --info --verbose --quiet --debug $pskc_all > tmp 2>&1

diff -ur $pskc_all tmp

$PSKCTOOL --info --debug $pskc_all > tmp 2>&1

diff -ur $pskc_all_human tmp

rm -f tmp

exit 0
