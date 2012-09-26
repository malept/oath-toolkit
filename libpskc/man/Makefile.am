# Copyright (C) 2012 Simon Josefsson

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

dist_man_MANS = $(gdoc_MANS)
MAINTAINERCLEANFILES = $(dist_man_MANS)

GDOC_BIN = $(srcdir)/gdoc
GDOC_SRC = $(top_srcdir)/global.c $(top_srcdir)/errors.c	\
	$(top_srcdir)/data.c $(top_srcdir)/validate.c		\
	$(top_srcdir)/output.c

GDOC_MAN_EXTRA_ARGS = -module $(PACKAGE) -sourceversion $(VERSION) \
        -bugsto $(PACKAGE_BUGREPORT) -pkg-name "$(PACKAGE_NAME)" \
        -includefuncprefix \
        -copyright "2012 Simon Josefsson" \
        -verbatimcopying

include $(srcdir)/gdoc.mk
