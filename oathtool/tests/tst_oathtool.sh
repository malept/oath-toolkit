#!/bin/sh

# tst_oathtool.sh - test that oathtool works
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

OATHTOOL=../oathtool

ME_=`expr "./$0" : '.*/\(.*\)$'`

Exit () { set +e; (exit $1); exit $1; }
fail_() { echo "$ME_: failed test: $@" 1>&2; Exit 1; }
skip_() { echo "$ME_: skipped test: $@" 1>&2; Exit 77; }

dotest()
{
    case $# in
	2);;
	3);;
	*) fail_ "internal error";;
    esac

    params="$1"
    expect="$2"
    if test -n "$3"; then
	cmp="="
    else
	cmp="!="
    fi

    got="`$OATHTOOL $params 2> /dev/null`"
    err="`$OATHTOOL $params 2>&1 > /dev/null`"

    if test "`echo $got`" $cmp "$expect"; then
	echo FAIL: oathtool $params
	echo expected: -$expect-
	echo got: -$got-
	echo err: -$err-
	exit 1
    else
	echo PASS oathtool $params
    fi
}

sha1key=3132333435363738393031323334353637383930
sha256key=3132333435363738393031323334353637383930313233343536373839303132
sha512key=31323334353637383930313233343536373839303132333435363738393031323334353637383930313233343536373839303132333435363738393031323334

dotest "--version" "fail" fail
dotest "" "fail" fail
dotest "-h" "fail" fail
dotest "-z" "fail" fail
dotest "Z" "fail" fail
dotest "00" "328482"
dotest "--base32 GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ" "755224"
dotest "--base32 gezdgnbvgy3tqojqgezdgnbvgy3tqojq" "755224"
dotest "--base32 gr6d5br725s6vnckv4vlhlaore" "993210"
dotest "-d 6 00" "328482"
dotest "-d 9 00" "fail" fail
dotest "--digits=6 00" "328482"
dotest "--digits 6 00" "328482"
dotest "--digits 7 00" "5328482"
dotest "--digits 8 00" "35328482"
dotest "--counter 0 --digits 8 00" "35328482"
dotest "-c 1 --digits 8 00" "30812658"
dotest "--counter 1 --digits 8 00" "30812658"
dotest "--window 4 -d 7 00" "5328482 0812658 1073348 1887919 2320986"
dotest "--window 4 -c 4 00" "320986 435986 964213 267638 985814"
dotest "00 328482" "0"
dotest "-c 4 00 964213" "2" fail
dotest "-c 4 -w 4 00 964213" "2"
dotest "-c 4 -w 4 -d 6 00 964213" "2"
dotest "-c 4 -w 4 -d 7 00 964213" "2" fail
dotest "-c 4 -d 7 00 964213" "2" fail
dotest "--totp $sha1key -N @59 -d8" "94287082"
dotest "--totp=sha256 $sha256key -N @59 -d8" "46119246"
dotest "--totp=sha512 $sha512key -N @59 -d8" "90693936"
dotest "--totp $sha1key -N @1111111109 -d8" "07081804"
dotest "--totp=sha256 $sha256key -N @1111111109 -d8" "68084774"
dotest "--totp=sha512 $sha512key -N @1111111109 -d8" "25091201"
dotest "--totp $sha1key -N @1111111111 -d8" "14050471"
dotest "--totp=sha256 $sha256key -N @1111111111 -d8" "67062674"
dotest "--totp=sha512 $sha512key -N @1111111111 -d8" "99943326"
dotest "--totp $sha1key -N @1234567890 -d8" "89005924"
dotest "--totp=sha256 $sha256key -N @1234567890 -d8" "91819424"
dotest "--totp=sha512 $sha512key -N @1234567890 -d8" "93441116"
dotest "--totp $sha1key -N @2000000000 -d8" "69279037"
dotest "--totp=sha256 $sha256key -N @2000000000 -d8" "90698825"
dotest "--totp=sha512 $sha512key -N @2000000000 -d8" "38618901"
dotest "--totp 00 --time-step-size=45m --start-time=now --now=now" "328482"
dotest "--totp 00 --time-step-size=1m --start-time=now --now=hour -d7" "4511481"
dotest "--totp --now @1111111109 -w 10 $sha1key 07081804" "0"
dotest "--totp=sha256 --now @1111111109 -w 10 $sha256key 68084774" "0"
dotest "--totp --now @1111111109 -w 10 $sha1key 14050471" "1"
dotest "--totp --now @1111111109 -w 5 $sha1key" "081804 050471 266759 306183 466594 754889"
dotest "--totp=sha256 --now @1111111109 -w 5 $sha256key" "084774 062674 267535 096086 328915 956967"
dotest "--hotp --counter 1099511627776 00" "363425"

exit 0
