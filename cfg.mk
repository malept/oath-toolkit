# Copyright (C) 2009, 2010, 2011 Simon Josefsson

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

CFGFLAGS = --enable-gtk-doc --enable-gtk-doc-pdf

ifeq ($(.DEFAULT_GOAL),abort-due-to-no-makefile)
.DEFAULT_GOAL := buildit
endif

local-checks-to-skip = sc_GPL_version sc_immutable_NEWS sc_prohibit_strcmp
VC_LIST_ALWAYS_EXCLUDE_REGEX = ^(oathtool|liboath)/(m4|lib)$

# Re-add when we have translation
local-checks-to-skip += sc_unmarked_diagnostics sc_bindtextdomain

update-copyright-env = UPDATE_COPYRIGHT_HOLDER="Simon Josefsson"
buildit:
	test -f configure || autoreconf --force --install
	test -f Makefile || ./configure $(CFGFLAGS)
	make

glimport:
	gnulib-tool --add-import
	rm -rfv lib
	cd liboath && gnulib-tool --add-import
	cd oathtool && gnulib-tool --add-import

ChangeLog:
	git2cl > ChangeLog
	cat .clcopying >> ChangeLog

web-coverage:
	rm -fv `find $(htmldir)/coverage -type f | grep -v CVS`
	cp -rv $(COVERAGE_OUT)/* $(htmldir)/coverage/

upload-web-coverage:
	cd $(htmldir) && \
		cvs commit -m "Update." coverage

tag = $(PACKAGE)-`echo $(VERSION) | sed 's/\./-/g'`
htmldir = ../www-$(PACKAGE)

release: prepare upload web upload-web

prepare:
	test `git describe` = `git tag -l $(tag)`
	rm -f ChangeLog
	$(MAKE) ChangeLog distcheck
	gpg -b $(distdir).tar.gz
	gpg --verify $(distdir).tar.gz.sig
	git commit -m Generated. ChangeLog
	git tag -f -u b565716f! -m $(VERSION) $(tag)

upload:
	git push
	git push --tags
	cp $(distdir).tar.gz $(distdir).tar.gz.sig ../releases/$(PACKAGE)/
	scp $(distdir).tar.gz $(distdir).tar.gz.sig jas@dl.sv.nongnu.org:/releases/oath-toolkit/

web:
	groff -man -T html oathtool/oathtool.1  > $(htmldir)/man-oathtool.html
	cp -v liboath/gtk-doc/html/*.html liboath/gtk-doc/html/*.png liboath/gtk-doc/html/*.devhelp liboath/gtk-doc/html/*.css $(htmldir)/reference/

upload-web:
	cd $(htmldir) && \
		cvs commit -m "Update." reference/
