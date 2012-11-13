#src makefile
lib_LTLIBRARIES = libgtweet.la

pkginclude_HEADERS = tweet-object.h \
			tweet.h \
			tweet-curl.h \
			tweet-json.h \
			tweet-oauth.h \
			tweet-twitter.h

libgtweet_la_SOURCES = tweet-object.c \
			tweet-curl.c \
			tweet-json.c \
			tweet-oauth.c \
			tweet-twitter.c

libgtweet_la_CFLAGS = $(PKGCFG_CFLAGS)
libgtweet_la_LDFLAGS = $(PKGCFG_LIBS)

if HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INTROSPECTION_GIRS = Gtweet-@VERSION@.gir
Gtweet_0_3_gir_INCLUDES = GObject-2.0 Gio-2.0
Gtweet_0_3_gir_LIBS = libgtweet.la
Gtweet_0_3_gir_PACKAGES = @packages@
Gtweet_0_3_gir_FILES = $(pkginclude_HEADERS) $(libgtweet_la_SOURCES)

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:%.gir=%.typelib)

$(gir_DATA): libgtweet.la
$(typelib_DATA): $(gir_DATA)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

endif