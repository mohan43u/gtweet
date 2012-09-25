# makefile for tweetpts and libtweetpts

tweetptsincludedir = $(includedir)/tweetpts

tweetptsinclude_HEADERS = tweetpts.h 	\
	tweetpts-curlapi.h 		\
	tweetpts-cursesapi.h 		\
	tweetpts-glibapi.h 		\
	tweetpts-jsonapi.h 		\
	tweetpts-oauthapi.h 		\
	tweetpts-readlineapi.h 		\
	tweetpts-twitterapi.h
lib_LTLIBRARIES = libtweetpts.la
libtweetpts_la_SOURCES = tweetpts-curlapi.c 	\
	tweetpts-cursesapi-display.c 		\
	tweetpts-cursesapi-keyboard.c 		\
	tweetpts-glibapi.c 			\
	tweetpts-jsonapi.c 			\
	tweetpts-oauthapi.c 			\
	tweetpts-readlineapi.c 			\
	tweetpts-twitterapi.c
libtweetpts_la_CFLAGS = $(PKGCFG_CFLAGS)
libtweetpts_la_LDFLAGS = $(PKGCFG_LIBS) -lreadline

bin_PROGRAMS = tweetpts
tweetpts_SOURCES = tweetpts.c
tweetpts_CFLAGS = $(PKGCFG_CFLAGS)
tweetpts_LDADD = libtweetpts.la