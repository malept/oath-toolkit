#!/bin/sh

# tst_hotptool.sh - test that hotptool works
# Copyright (C) 2009  Simon Josefsson
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see
# <http://www.gnu.org/licenses/>.

HOTPTOOL=../hotptool

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

    got="`$HOTPTOOL $params 2> /dev/null`"
    err="`$HOTPTOOL $params 2>&1 > /dev/null`"

    if test "`echo $got`" $cmp "$expect"; then
	echo FAIL: hotptool $params
	echo expected: -$expect-
	echo got: -$got-
	echo err: -$err-
	exit 1
    else
	echo PASS hotptool $params
    fi
}

dotest "--version" "fail" fail
dotest "" "fail" fail
dotest "-h" "fail" fail
dotest "-z" "fail" fail
dotest "Z" "fail" fail
dotest "00" "328482"
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

exit 0
