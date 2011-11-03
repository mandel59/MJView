fontsdir = $(datadir)/mjview/fonts
fonts_DATA = ipamjm.ttf

ipamjm00101.zip:
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/ipamjm00101.php

ipamjm.ttf: ipamjm00101.zip
	unzip -p $< */ipamjm.ttf > $@

dbdir = $(datadir)/mjview/db
db_DATA = mj.db

MJMojiJouhouIchiranhyou00101.zip:
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/mjmojiichiran/MJMojiJouhouIchiranhyou00101.zip

mj.db:  MJMojiJouhouIchiranhyou00101.zip split_charinfo.pl
	rm -f $@
	unzip -p $< MJ*.csv | perl $(word 2, $^) $@

EXTRA_DIST = split_charinfo.pl

DISTCLEANFILES = \
	ipamjm.ttf \
	mj.db

uninstall-local:
	-rm -r $(fontsdir)
	-rm -r $(dbdir)

