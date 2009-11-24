SUBDIRS = lib tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -DLIBHOTP_BUILDING
AM_CPPFLAGS += -I$(srcdir)/lib -I$(builddir)/lib

AM_CFLAGS = $(CFLAG_VISIBILITY)

lib_LTLIBRARIES = libhotp.la

libhotp_la_SOURCES = libhotp.map hotp.c hotp.h
libhotp_la_LIBADD = lib/libgnu.la
libhotp_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -no-undefined

if HAVE_LD_VERSION_SCRIPT
libhotp_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libhotp.map
else
libhotp_la_LDFLAGS += -export-symbols-regex '^hotp_.*'
endif
