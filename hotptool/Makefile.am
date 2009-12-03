SUBDIRS = lib . tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)/../libhotp -I$(srcdir)/lib -I$(builddir)/lib

bin_PROGRAMS = hotptool
hotptool_SOURCES = hotptool.c
hotptool_LDADD = ../libhotp/libhotp.la lib/libgnu.la libhotptool_cmd.la

noinst_LTLIBRARIES = libhotptool_cmd.la
BUILT_SOURCES = hotptool_cmd.c hotptool_cmd.h
libhotptool_cmd_la_SOURCES = $(BUILT_SOURCES) hotptool.ggo
libhotptool_cmd_la_LIBADD = lib/libgnu.la
libhotptool_cmd_la_CFLAGS =

$(BUILT_SOURCES): hotptool.ggo Makefile.am
	gengetopt --unamed-opts="KEY [OTP]" \
		--no-handle-version --no-handle-help \
		--input $< --file-name hotptool_cmd

dist_man_MANS = hotptool.1
MAINTAINERCLEANFILES = $(dist_man_MANS)

hotptool.1: $(srcdir)/hotptool.c $(srcdir)/hotptool.ggo $(srcdir)/configure.ac
	$(HELP2MAN) \
		--name="HOTP one-time password tool" \
		--output=$@ $(top_builddir)/hotptool$(EXEEXT)
