#!/bin/sh

# tst_libexamples.sh - keep pskctool output in GTK-DOC manual up to date
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

srcdir=${srcdir:-.}
PSKCTOOL=../pskctool

$PSKCTOOL -h | sed '1,2d' > foo
if ! diff -ur $srcdir/../../libpskc/examples/pskctool-h.txt foo; then
    cp foo $srcdir/../../libpskc/examples/pskctool-h.txt
    echo "FAIL: pskctool --help output change, commit updated file."
    exit 1
fi

$PSKCTOOL -i $srcdir/../../libpskc/examples/pskc-hotp.xml > foo
if ! diff -ur $srcdir/../../libpskc/examples/pskc-hotp-human.txt foo; then
    cp foo $srcdir/../../libpskc/examples/pskc-hotp-human.txt
    echo "FAIL: pskctool --info output change, commit updated file."
    exit 1
fi

$PSKCTOOL --sign --sign-key $srcdir/pskc-ee-key.pem \
    --sign-crt $srcdir/pskc-ee-crt.pem \
    $srcdir/../../libpskc/examples/pskc-hotp.xml > foo
if ! diff -ur $srcdir/../../libpskc/examples/pskc-hotp-signed.xml foo; then
    cp foo $srcdir/../../libpskc/examples/pskc-hotp-signed.xml
    echo "FAIL: pskctool --sign output change, commit updated file."
    exit 1
fi

$PSKCTOOL --info --debug --quiet $srcdir/pskc-figure6.xml > foo 2>&1
if ! diff -ur $srcdir/../../libpskc/examples/pskc-figure6-debug.txt foo; then
    cp foo $srcdir/../../libpskc/examples/pskc-figure6-debug.txt
    echo "FAIL: pskctool --info output change, commit updated file."
    exit 1
fi

rm -f foo

exit 0
