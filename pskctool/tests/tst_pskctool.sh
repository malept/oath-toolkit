#!/bin/sh

# tst_pskctool.sh - test that pskctool works
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

    got="`$PSKCTOOL $params 2> /dev/null || true`"
    err="`$PSKCTOOL $params 2>&1 > /dev/null || true`"

    if test "`echo $got`" $cmp "$expect"; then
	echo FAIL: pskctool $params
	echo expected: -$expect-
	echo got: -$got-
	echo err: -$err-
	exit 1
    else
	echo PASS pskctool $params
    fi
}

$PSKCTOOL --version > /dev/null
$PSKCTOOL --help > /dev/null
$PSKCTOOL -h > /dev/null

dotest "--version" "fail" fail
dotest "" "fail" fail
dotest "-h" "fail" fail

dotest "--info $srcdir/tst_pskctool.sh" "fail" fail
dotest "--info --quiet --debug --strict $srcdir/tst_pskctool.sh" "fail" fail
dotest "--info --strict $srcdir/pskc-invalid.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-figure2.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-figure3.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-figure4.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-figure5.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-figure10.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-all.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-ns.xml" ""
dotest "--info --quiet --debug --strict $srcdir/pskc-ocra.xml" ""
dotest "--info --quiet --debug --strict $srcdir/20120919-test001-4282.xml" ""
dotest "--validate --quiet --debug --strict $srcdir/tst_pskctool.sh" "fail" fail
dotest "--validate $srcdir/tst_pskctool.sh" "fail" fail
dotest "--validate $srcdir/pskc-invalid.xml" "FAIL"
dotest "--validate --quiet --debug --strict $srcdir/pskc-invalid.xml" "FAIL" fail
dotest "--validate $srcdir/pskc-figure2.xml" "OK"
dotest "--validate $srcdir/pskc-figure3.xml" "OK"
dotest "--validate $srcdir/pskc-figure4.xml" "OK"
dotest "--validate $srcdir/pskc-figure5.xml" "OK"
dotest "--validate $srcdir/pskc-figure10.xml" "OK"
dotest "--validate $srcdir/pskc-all.xml" "OK"
dotest "--validate $srcdir/pskc-ns.xml" "OK"
dotest "--validate $srcdir/pskc-ocra.xml" "OK"
dotest "--validate $srcdir/20120919-test001-4282.xml" "OK"

exit 0
