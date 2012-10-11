<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method="xml" encoding="UTF-8" indent="yes"
	      doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
	      doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>
  <xsl:template match="/">

<html>
  <head>
    <link rel="stylesheet" href="asciidoc.css" type="text/css" />
    <link rel="stylesheet" href="layout.css" type="text/css" />
    <xsl:copy-of select="//head/*" />
    <script type="text/javascript">
      /* <![CDATA[ */
      (function() {
      var s = document.createElement('script'), t = document.getElementsByTagName('script')[0];
      s.type = 'text/javascript';
      s.async = true;
      s.src = 'http://api.flattr.com/js/0.6/load.js?mode=auto';
      t.parentNode.insertBefore(s, t);
      })();
      /* ]]> */</script>
  </head>
  <body>
    <div id="layout-menu-box">
      <div id="layout-menu">
	<div>&#187;<a href="index.html">Home</a></div>
	<div>&#187;<a href="NEWS.html">News</a></div>
	<div>&#187;<a href="download.html">Download</a></div>
	<div>&#187;<a href="docs.html">Documentation</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="oathtool.1.html">oathtool(1)</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="libpskc-api/pskc-tutorial-pskctool.html">pskctool(1)</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="liboath-api/liboath-oath.html">Liboath&#160;API</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="libpskc-api/pskc-tutorial-quickstart.html">Libpskc Quickstart</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="libpskc-api/pskc-reference.html">Libpskc&#160;API</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="pam_oath.html">pam_oath</a></div>
	<div>&#187;<a href="contrib.html">Contribute</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="http://git.savannah.gnu.org/cgit/oath-toolkit.git/tree">Code</a></div>
	<div>&#160;&#160;&#160;&#160;&#160;&#187;<a href="https://savannah.nongnu.org/support/?group=oath-toolkit">Bug&#160;tracker</a></div>
	<div style="text-align:center;padding-top: 1em;">
	  <a class="FlattrButton" style="display:none;" href="http://www.nongnu.org/oath-toolkit/"></a>
	  <noscript><div><a href="http://flattr.com/thing/130057/OATH-Toolkit">
	  <img src="http://api.flattr.com/button/flattr-badge-large.png" alt="Flattr this" title="Flattr this" style="border:0" /></a></div></noscript>
	</div>
      </div>
    </div>
    <div id="layout-content-box">
      <div id="layout-banner">
	<div id="layout-title">OATH Toolkit</div>
	<div id="layout-description">One-time password components</div>
      </div>
      <div id="layout-content">
	<div id="header">
	  <xsl:copy-of select="//div[@id='header']/*" />
	</div>
	<div id="content">
	    <xsl:choose>
		<xsl:when test="//div[@id='content']">
		    <xsl:copy-of select="//div[@id='content']/*" />
		</xsl:when>
		<xsl:otherwise>
		    <xsl:copy-of select="//body/*" />
		</xsl:otherwise>
	    </xsl:choose>
	</div>
	<div id="footer">
	  <div id="footer-badges">
	    <a href="http://validator.w3.org/check?uri=referer">
	      <img style="border:0;width:88px;height:31px"
		   src="http://www.w3.org/Icons/valid-xhtml10-blue"
		   alt="Valid XHTML 1.0" height="31" width="88" />
	    </a>
	    <a href="http://jigsaw.w3.org/css-validator/">
	      <img style="border:0;width:88px;height:31px"
		   src="http://jigsaw.w3.org/css-validator/images/vcss-blue"
		   alt="Valid CSS!" />
	    </a>
	  </div>
	</div>
      </div>
    </div>
  </body>
</html>

  </xsl:template>
</xsl:stylesheet>
