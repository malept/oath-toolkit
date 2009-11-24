SUBDIRS = . tests

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I. -I$(srcdir)/../libhotp
AM_CFLAGS = -DDEBUG_PAM

libdir = $(PAMDIR)

lib_LTLIBRARIES = pam_hotp.la

pam_hotp_la_SOURCES = pam_hotp.c
# XXX add -Wl,-x too?  PAM documentation suggests it.
pam_hotp_la_LIBADD = ../libhotp/libhotp.la
pam_hotp_la_LDFLAGS = -module -avoid-version
