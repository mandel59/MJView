## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po data

mjviewdocdir = $(docdir)
mjviewdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(mjviewdoc_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache

uninstall-hook:
	-rmdir -p $(mjviewdocdir)

