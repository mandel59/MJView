CC        = clang
CFLAGS    = -O -I ./src `pkg-config --cflags freetype2 cairo`
OBJS      = src/mjview.o src/ft_cairo.o
LIBS      = `pkg-config --libs freetype2 cairo`
BUILD_DIR = bin

all: $(BUILD_DIR)/ipamjm.ttf $(BUILD_DIR)/mjcharinfo.db $(BUILD_DIR)/mjview

data/ipamjm00101.zip:
	mkdir -p data
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/ipamjm00101.php
data/MJMojiJouhouIchiranhyou00101.zip:
	mkdir -p data
	wget -O $@ http://ossipedia.ipa.go.jp/ipamjfont/mjmojiichiran/MJMojiJouhouIchiranhyou00101.zip

$(BUILD_DIR)/mjview: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

$(BUILD_DIR)/ipamjm.ttf: data/ipamjm00101.zip
	mkdir -p $(BUILD_DIR)
	unzip -p $< */ipamjm.ttf > $@

$(BUILD_DIR)/mjcharinfo.db: data/MJMojiJouhouIchiranhyou00101.zip src/split_charinfo.pl
	mkdir -p $(BUILD_DIR)
	rm -f $@
	unzip -p $< MJ*.csv | perl src/split_charinfo.pl $@

run: all
	IPAMJM=$(BUILD_DIR)/ipamjm.ttf $(BUILD_DIR)/mjview > test.svg

clean:
	rm -f src/*.o
	rm -rf $(BUILD_DIR)

.PHONY: all run clean

