ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I. -I$(top_srcdir)/src -I$(top_srcdir)/compat -I$(top_builddir)/src

lib_LTLIBRARIES = libmicrodns.la
noinst_LTLIBRARIES = libcompat.la

libcompat_la_SOURCES = compat/compat.c compat/compat.h compat/inet.c compat/poll.c

libmicrodns_la_SOURCES = \
	src/mdns.c \
	src/microdns.h \
	src/rr.c src/rr.h \
	src/utils.h

libmicrodns_la_LDFLAGS = -no-undefined -export-symbols-regex '^(mdns|rr)_'
libmicrodns_la_LIBADD = libcompat.la $(LIBSOCKET)

EXTRA_PROGRAMS = test announce

test_SOURCES = examples/main.c
test_LDADD = libmicrodns.la
announce_SOURCES = examples/announce.c
announce_LDADD = libcompat.la libmicrodns.la

pkginclude_HEADERS = src/microdns.h src/rr.h
pkgconfig_DATA = src/microdns.pc

dist_doc_DATA = README.md
