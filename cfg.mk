# Copyright (C) 2009, 2010  Simon Josefsson

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
VC_LIST_ALWAYS_EXCLUDE_REGEX = ^(hotptool|libhotp)/(m4|lib)$

# Re-add when we have translation
local-checks-to-skip += sc_unmarked_diagnostics sc_bindtextdomain

buildit:
	test -f configure || autoreconf --force --install
	test -f Makefile || ./configure $(CFGFLAGS)
	make

glimport:
	gnulib-tool --add-import
	rm -rfv lib
	cd libhotp && gnulib-tool --add-import
	cd hotptool && gnulib-tool --add-import

ChangeLog:
	git2cl > ChangeLog
	cat .clcopying >> ChangeLog

tag = $(PACKAGE)-`echo $(VERSION) | sed 's/\./-/g'`
htmldir = ../www-$(PACKAGE)

release: prepare upload web upload-web

prepare:
	! git tag -l $(tag) | grep $(PACKAGE) > /dev/null
	rm -f ChangeLog
	$(MAKE) ChangeLog distcheck
	gpg -b $(distdir).tar.gz
	gpg --verify $(distdir).tar.gz.sig
	git commit -m Generated. ChangeLog
	git tag -u b565716f! -m $(VERSION) $(tag)

upload:
	git push
	git push --tags
	cp $(distdir).tar.gz $(distdir).tar.gz.sig ../releases/$(PACKAGE)/

web:
	cp -v libhotp/gtk-doc/html/*.html libhotp/gtk-doc/html/*.png libhotp/gtk-doc/html/html/*.devhelp libhotp/gtk-doc/html/*.css $(htmldir)/reference/

upload-web:
	cd $(htmldir) && \
		cvs commit -m "Update." reference/
