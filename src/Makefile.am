## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(datadir)/mjview/ui
ui_DATA = mjview.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_TOP_BUILDDIR=\""$(abs_top_builddir)"\"\
	$(MJVIEW_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mjview

mjview_SOURCES = \
	main.c \
	mjview.h \
	mjview.c \
	ft_cairo.h \
	ft_cairo.c	

mjview_LDFLAGS = \
	-Wl,--export-dynamic

mjview_LDADD = $(MJVIEW_LIBS)


EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/mjview

