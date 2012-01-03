#!/bin/sh
# tst_usersfile.sh - Invoke tst_usersfile and check output.
# Copyright (C) 2009-2012 Simon Josefsson

# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

srcdir=${srcdir:-.}

cp $srcdir/users.oath tmp.oath

TZ=UTC
export TZ

TSTAMP=`datefudge "2006-09-23" date -u +%s`
if test "$TSTAMP" != "1158969600"; then
    echo "Could not fake timestamps with datefudge."
    ./tst_usersfile$EXEEXT
    rc=$?
else
    datefudge 2006-12-07 ./tst_usersfile$EXEEXT
    rc=$?
    sed 's/2006-12-07T00:00:0.L/2006-12-07T00:00:00L/g' < tmp.oath > tmp2.oath
    diff -ur $srcdir/expect.oath tmp2.oath || rc=1
fi

rm -f tmp.oath tmp2.oath

exit $rc
