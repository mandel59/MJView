fontsdir = $(datadir)/mjview/fonts
fonts_DATA = ipamjm.ttf

dbdir = $(datadir)/mjview/db
db_DATA = mj.db

desktopdir = $(datadir)/applications
desktop_in_files = mjview.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

mjviewdatadocdir = $(docdir)
mjviewdatadoc_DATA = \
	COPYING_IPAFONT \
	COPYING_CC

uidir = $(datadir)/mjview/ui
ui_DATA = mjview.ui

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

EXTRA_DIST = \
	$(desktop_in_files) \
	$(ui_DATA) \
	COPYING_CC \
	ipamjm00101.zip \
	MJMojiJouhouIchiranhyou00101.zip \
	split_charinfo.pl

DISTCLEANFILES = \
	$(desktop_DATA) \
	COPYING_IPAFONT \
	ipamjm.ttf \
	mj.db

uninstall-hook:
	-rmdir -p $(uidir)
	-rmdir -p $(fontsdir)
	-rmdir -p $(dbdir)
	-rmdir -p $(mjviewdatadocdir)

