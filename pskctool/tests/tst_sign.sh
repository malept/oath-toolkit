#!/bin/sh

# tst_sign.sh - test that pskctool can sign and verify
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

PSKCTOOL=../pskctool

srcdir="${srcdir:-.}"
pskc_all="$srcdir/pskc-all.xml"
pskc_all_signed="$srcdir/pskc-all-signed.xml"
pskc_ee_key="$srcdir/pskc-ee-key.pem"
pskc_ee_crt="$srcdir/pskc-ee-crt.pem"
pskc_root_crt="$srcdir/pskc-root-crt.pem"

$PSKCTOOL --info --strict --debug $pskc_all > tmp-pre-human.txt

$PSKCTOOL --sign \
    --sign-key $pskc_ee_key \
    --sign-crt $pskc_ee_crt \
    $pskc_all > tmp-signed.xml

diff -ur $pskc_all_signed tmp-signed.xml

$PSKCTOOL --info --strict --debug tmp-signed.xml \
    | sed 's/Signed: YES/Signed: NO/' > tmp-post-human.txt

diff -ur tmp-pre-human.txt tmp-post-human.txt

$PSKCTOOL --verify --quiet \
    --verify-crt $pskc_root_crt \
    tmp-signed.xml

rm -f tmp-pre-human.txt tmp-signed.xml tmp-post-human.txt

exit 0
