#!/bin/sh

(
    echo 'OATH Toolkit NEWS'
    echo '================='
    echo ''
    cat ../NEWS | sed -n '/(released/,$ p' | sed 's/^* Version/.Version/g'
) > NEWS.txt

(
    echo 'Contribute'
    echo '=========='
    echo ''
    cat ../HACKING | sed -e '1,2d'
) > contrib.txt

(
    echo 'Manual for pam_oath'
    echo '==================='
    echo ''
    cat ../pam_oath/README | sed -e '1,5d'
) > pam_oath.txt

(
    LAST_VERSION=$(grep '^\* Version.*(released' ../NEWS |head -1|sed -n -e 's/^* Version \(.*\) (released \(.*\))/\1/;p')
    LAST_DATE=$(grep '^\* Version.*(released' ../NEWS |head -1|sed -n -e 's/^* Version \(.*\) (released \(.*\))/\2/;p')
    echo 'OATH Toolkit'
    echo '============'
    echo ''
    echo ".$LAST_DATE: OATH Toolkit $LAST_VERSION Released"
    cat intro.txt
) > index.txt

for txt in index.txt NEWS.txt download.txt docs.txt pam_oath.txt contrib.txt; do
    html=`echo $txt | sed 's/\.txt$/.html/'`
    asciidoc --backend=xhtml11 --conf-file=config.cfg -a index-only -o tmp $txt
    tr -d '\015' < tmp > tmp2
    xsltproc --html --output $html oath-toolkit.xsl tmp2
done

for man in ../oathtool/oathtool.1 ../pskctool/pskctool.1; do
    html=`basename $man`.html
    groff -man -T html $man \
	| grep -v -e '<meta http-equiv' -e 'text-align: center' \
	| sed 's/ align="center"//' > tmp
    xsltproc --html --output $html oath-toolkit.xsl tmp
done

for lib in oath pskc; do
    mkdir -p lib$lib-api/
    cp ../lib$lib/gtk-doc/html/*.png lib$lib-api/
    for src in `find ../lib$lib/gtk-doc/html/ -name "*.html" -print`; do
	dst=`basename $src`
	xsltproc --html --output tmp oath-toolkit.xsl $src
	sed \
	    -e 's#»<a href="#»<a href="../#g' \
	    -e 's#"asciidoc.css"#"../asciidoc.css"#' \
	    -e 's#"style.css"#"../style.css"#' \
	    -e 's#"layout.css"#"../layout.css"#' < tmp > lib$lib-api/$dst
    done
done

rm -f tmp tmp2
