#!/bin/sh

# tst_libexamples.sh - keep pskctool output in GTK-DOC manual up to date
# Copyright (C) 2012 Simon Josefsson

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

srcdir=${srcdir:-.}
PSKCTOOL=../pskctool

$PSKCTOOL -h | sed '1,2d' > foo
if ! diff -ur $srcdir/../../libpskc/examples/pskctool-h.txt foo; then
    cp foo $srcdir/../../libpskc/examples/pskctool-h.txt
    echo "FAIL: pskctool --help output change, commit updated file."
    exit 1
fi

$PSKCTOOL -c $srcdir/../../libpskc/examples/pskc-hotp.xml | sed '$d' > foo
if ! diff -ur $srcdir/../../libpskc/examples/pskc-hotp-human.txt foo; then
    cp foo $srcdir/../../libpskc/examples/pskc-hotp-human.txt
    echo "FAIL: pskctool --check output change, commit updated file."
    exit 1
fi

rm -f foo

exit 0
