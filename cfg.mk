# Copyright (C) 2009-2012 Simon Josefsson

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

CFGFLAGS = --enable-gtk-doc --enable-gtk-doc-pdf --enable-gcc-warnings	\
	--enable-root-tests

ifeq ($(.DEFAULT_GOAL),abort-due-to-no-makefile)
.DEFAULT_GOAL := bootstrap
endif

INDENT_SOURCES = `find . -name '*.[ch]' | grep -v -e /gl/ -e build-aux`

bootstrap:
	printf "gdoc_MANS =\ngdoc_TEXINFOS =\n" > liboath/man/Makefile.gdoc
	printf "gdoc_MANS =\ngdoc_TEXINFOS =\n" > libpskc/man/Makefile.gdoc
	touch ChangeLog
	test -f configure || autoreconf --force --install
	test -f Makefile || ./configure $(CFGFLAGS)
	make

# syntax-check
VC_LIST_ALWAYS_EXCLUDE_REGEX = ^GNUmakefile|maint.mk|build-aux/|gl/|m4/libxml2.m4|oathtool/doc/parse-datetime.texi|(liboath|libpskc)/man/gdoc|liboath/gtk-doc.make|libpskc/gtk-doc.make|libpskc/schemas/|(oathtool|liboath)/(build-aux|gl)/.*$$
# syntax-check: Project wide exceptions on philosophical grounds.
local-checks-to-skip = sc_GPL_version sc_immutable_NEWS	\
	sc_prohibit_strcmp
# syntax-check: Re-add when we have translation.
local-checks-to-skip += sc_unmarked_diagnostics sc_bindtextdomain
# syntax-check: Revisit these soon.
local-checks-to-skip += sc_prohibit_atoi_atof
# syntax-check: Explicit syntax-check exceptions.
exclude_file_name_regexp--sc_program_name = ^liboath/tests/|libpskc/examples/|libpskc/tests/|pam_oath/tests/
exclude_file_name_regexp--sc_texinfo_acronym = ^oathtool/doc/parse-datetime.texi
exclude_file_name_regexp--sc_error_message_uppercase = ^oathtool/oathtool.c|pskctool/pskctool.c
exclude_file_name_regexp--sc_require_config_h = ^libpskc/examples/
exclude_file_name_regexp--sc_require_config_h_first = $(exclude_file_name_regexp--sc_require_config_h)
exclude_file_name_regexp--sc_trailing_blank = ^libpskc/examples/pskctool-h.txt
exclude_file_name_regexp--sc_two_space_separator_in_usage = ^pskctool/tests/

update-copyright-env = UPDATE_COPYRIGHT_HOLDER="Simon Josefsson" UPDATE_COPYRIGHT_USE_INTERVALS=1

glimport:
	gnulib-tool --add-import
	cd liboath && gnulib-tool --add-import
	cd oathtool && gnulib-tool --add-import
	cd libpskc && gnulib-tool --add-import
	cd pskctool && gnulib-tool --add-import

# Release

tag = $(PACKAGE)-`echo $(VERSION) | sed 's/\./-/g'`
htmldir = ../www-$(PACKAGE)

coverage-my:
	$(MAKE) coverage WERROR_CFLAGS= VALGRIND=

coverage-copy:
	rm -fv `find $(htmldir)/coverage -type f | grep -v CVS`
	mkdir -p $(htmldir)/coverage/
	cp -rv $(COVERAGE_OUT)/* $(htmldir)/coverage/

coverage-upload:
	cd $(htmldir) && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec cvs add {} \; && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec sh -c "cvs add -kb {}/*.png" \; && \
	find coverage -type d -! -name CVS -! -name '.' \
		-exec sh -c "cvs add {}/*.html" \; && \
	cvs add coverage/$(PACKAGE).info coverage/gcov.css || true && \
	cvs commit -m "Update." coverage

clang:
	make clean
	scan-build ./configure
	rm -rf scan.tmp
	scan-build -o scan.tmp make

clang-copy:
	rm -fv `find $(htmldir)/clang-analyzer -type f | grep -v CVS`
	mkdir -p $(htmldir)/clang-analyzer/
	cp -rv scan.tmp/*/* $(htmldir)/clang-analyzer/

clang-upload:
	cd $(htmldir) && \
		cvs add clang-analyzer || true && \
		cvs add clang-analyzer/*.css clang-analyzer/*.js \
			clang-analyzer/*.html || true && \
		cvs commit -m "Update." clang-analyzer

ChangeLog:
	git2cl > ChangeLog
	cat .clcopying >> ChangeLog

tarball:
	test `git describe` = `git tag -l $(tag)`
	rm -f ChangeLog
	$(MAKE) ChangeLog distcheck

gtkdoc-copy:
	mkdir -p $(htmldir)/reference/ $(htmldir)/libpskc/
	cp -v liboath/gtk-doc/liboath.pdf \
		liboath/gtk-doc/html/*.html \
		liboath/gtk-doc/html/*.png \
		liboath/gtk-doc/html/*.devhelp2 \
		liboath/gtk-doc/html/*.css \
		$(htmldir)/reference/
	cp -v libpskc/gtk-doc/libpskc.pdf \
		libpskc/gtk-doc/html/*.html \
		libpskc/gtk-doc/html/*.png \
		libpskc/gtk-doc/html/*.devhelp2 \
		libpskc/gtk-doc/html/*.css \
		$(htmldir)/libpskc/

gtkdoc-upload:
	cd $(htmldir) && \
		cvs add reference || true && \
		cvs add -kb reference/*.png reference/*.pdf || true && \
		cvs add reference/*.html reference/*.css \
			reference/*.devhelp2 || true && \
		cvs commit -m "Update." reference/
	cd $(htmldir) && \
		cvs add libpskc || true && \
		cvs add -kb libpskc/*.png libpskc/*.pdf || true && \
		cvs add libpskc/*.html libpskc/*.css \
			libpskc/*.devhelp2 || true && \
		cvs commit -m "Update." libpskc/

man-copy:
	groff -man -T html oathtool/oathtool.1  > $(htmldir)/man-oathtool.html

man-upload:
	cd $(htmldir) && \
		cvs commit -m "Update." man-oathtool.html

.PHONY: website
website:
	cd website && ./build-website.sh

website-copy:
	mkdir -p $(htmldir)/liboath-api/ $(htmldir)/libpskc-api/
	cp website/*.html website/*.css $(htmldir)/
	cp website/liboath-api/*.html website/liboath-api/*.png \
		$(htmldir)/liboath-api/
	cp website/libpskc-api/*.html website/libpskc-api/*.png \
		$(htmldir)/libpskc-api/

website-upload:
	cd $(htmldir) && \
		cvs add *.html *.css || true && \
		cvs add liboath-api || true && \
		cvs add libpskc-api || true && \
		cvs add -kb liboath-api/*.png liboath-api/*.png || true && \
		cvs add -kb libpskc-api/*.png libpskc-api/*.png || true && \
		cvs add liboath-api/*.html || true && \
		cvs add libpskc-api/*.html || true && \
		cvs commit -m "Update."

release-check: syntax-check tarball man-copy gtkdoc-copy coverage-my coverage-copy clang clang-copy website website-copy

release-upload-www: man-upload gtkdoc-upload coverage-upload clang-upload website-upload

release-upload-ftp:
	gpg -b $(distdir).tar.gz
	gpg --verify $(distdir).tar.gz.sig
	cp $(distdir).tar.gz $(distdir).tar.gz.sig ../releases/$(PACKAGE)/
	scp $(distdir).tar.gz $(distdir).tar.gz.sig jas@dl.sv.nongnu.org:/releases/oath-toolkit/
	git push
	git push --tags

tag: # Use "make tag VERSION=1.2.3"
	git tag -s -u b565716f -m $(VERSION) $(tag)

release: release-check release-upload-www release-upload-ftp
