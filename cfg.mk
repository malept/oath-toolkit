# Copyright (C) 2009-2011 Simon Josefsson

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
.DEFAULT_GOAL := buildit
endif

buildit:
	test -f configure || autoreconf --force --install
	test -f Makefile || ./configure $(CFGFLAGS)
	make

# syntax-check
VC_LIST_ALWAYS_EXCLUDE_REGEX = ^maint.mk|(oathtool|liboath)/(m4|lib)/.*$$
# syntax-check: Project wide exceptions on philosophical grounds.
local-checks-to-skip = sc_GPL_version sc_immutable_NEWS	\
	sc_prohibit_strcmp
# syntax-check: Re-add when we have translation.
local-checks-to-skip += sc_unmarked_diagnostics sc_bindtextdomain
# syntax-check: Revisit these soon.
local-checks-to-skip += sc_prohibit_atoi_atof
# syntax-check: Explicit syntax-check exceptions.
exclude_file_name_regexp--sc_program_name = ^liboath/tests/|pam_oath/tests/
exclude_file_name_regexp--sc_texinfo_acronym = ^oathtool/doc/parse-datetime.texi
exclude_file_name_regexp--sc_error_message_uppercase = ^oathtool/oathtool.c

update-copyright-env = UPDATE_COPYRIGHT_HOLDER="Simon Josefsson" UPDATE_COPYRIGHT_USE_INTERVALS=1

glimport:
	gnulib-tool --add-import
	cd liboath && gnulib-tool --add-import
	cd oathtool && gnulib-tool --add-import

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
	mkdir -p $(htmldir)/reference/
	cp -v liboath/gtk-doc/liboath.pdf \
		liboath/gtk-doc/html/*.html \
		liboath/gtk-doc/html/*.png \
		liboath/gtk-doc/html/*.devhelp \
		liboath/gtk-doc/html/*.css \
		$(htmldir)/reference/

gtkdoc-upload:
	cd $(htmldir) && \
		cvs add reference || true && \
		cvs add -kb reference/*.png reference/*.pdf || true && \
		cvs add reference/*.html reference/*.css \
			reference/*.devhelp || true && \
		cvs commit -m "Update." reference/

man-copy:
	groff -man -T html oathtool/oathtool.1  > $(htmldir)/man-oathtool.html

man-upload:
	cd $(htmldir) && \
		cvs commit -m "Update." man-oathtool.html

source:
	-git commit -m Generated. ChangeLog
	git tag -u b565716f! -m $(VERSION) $(tag)

release-check: syntax-check tarball man-copy gtkdoc-copy coverage-my coverage-copy clang clang-copy

release-upload-www: man-upload gtkdoc-upload coverage-upload clang-upload

release-upload-ftp:
	git push
	git push --tags
	gpg -b $(distdir).tar.gz
	gpg --verify $(distdir).tar.gz.sig
	cp $(distdir).tar.gz $(distdir).tar.gz.sig ../releases/$(PACKAGE)/
	scp $(distdir).tar.gz $(distdir).tar.gz.sig jas@dl.sv.nongnu.org:/releases/oath-toolkit/

release: release-check release-upload-www source release-upload-ftp
