#exec makefile

bin_PROGRAMS = tweetclient
bin_SCRIPTS = tweetclientgtk.js
tweetclient_SOURCES = tweetclient.c
tweetclient_LDADD = @top_srcdir@/lib/libgtweet.la
tweetclient_CFLAGS = $(PKGCFG_CFLAGS) -I@top_srcdir@/lib/
tweetclient_LDFLAGS = $(PKGCFG_LIBS)
