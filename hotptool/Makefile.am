SUBDIRS = lib

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)/../libhotp -I$(srcdir)/lib -I$(builddir)/lib

bin_PROGRAMS = hotptool
hotptool_SOURCES = hotptool.c
hotptool_LDADD = ../libhotp/libhotp.la lib/libgnu.la
