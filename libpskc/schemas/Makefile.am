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

PSKC_URI = "urn:ietf:params:xml:ns:keyprov:pskc"
XDSIG_URI = "http://www.w3.org/TR/2002/REC-xmldsig-core-20020212/xmldsig-core-schema.xsd"
XENC_URI = "http://www.w3.org/TR/2002/REC-xmlenc-core-20021210/xenc-schema.xsd"

catalog-pskc.xml:
	xmlcatalog --create -noout \
		--add "system" $(PSKC_URI) \
			"$(abs_srcdir)/pskc-schema.xsd" \
		--add "system" $(XDSIG_URI) \
			"$(abs_srcdir)/xmldsig-core-schema.xsd" \
		--add "system" $(XENC_URI) \
			"$(abs_srcdir)/xenc-schema.xsd" \
	tmp
	mv tmp $@
