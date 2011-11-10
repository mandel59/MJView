fontsdir = $(datadir)/mjview/fonts
fonts_DATA = ipamjm.ttf

dbdir = $(datadir)/mjview/db
db_DATA = mj.db

mjviewdatadocdir = $(docdir)
mjviewdatadoc_DATA = \
	COPYING_IPAFONT \
	COPYING_CC

ipamjm00101.zip:
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/ipamjm00101.php

MJMojiJouhouIchiranhyou00101.zip:
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/mjmojiichiran/MJMojiJouhouIchiranhyou00101.zip

ipamjm.ttf: ipamjm00101.zip
	unzip -p $< */ipamjm.ttf > $@

COPYING_IPAFONT: ipamjm00101.zip
	unzip -p $< */IPA_Font_License_Agreement_v1.0.txt > $@

mj.db:  MJMojiJouhouIchiranhyou00101.zip split_charinfo.pl
	rm -f $@
	unzip -p $< MJ*.csv | perl $(word 2, $^) $@

EXTRA_DIST = ipamjm00101.zip MJMojiJouhouIchiranhyou00101.zip split_charinfo.pl COPYING_CC

DISTCLEANFILES = \
	COPYING_IPAFONT \
	ipamjm.ttf \
	mj.db

uninstall-hook:
	-rmdir -p $(fontsdir)
	-rmdir -p $(dbdir)
	-rmdir -p $(mjviewdatadocdir)

