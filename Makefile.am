# Makefile for tweetpts

include_HEADERS = tweetpts.h

bin_PROGRAMS = tweetpts
tweetpts_SOURCES = tweetpts.c oauth.c curl.c xml.c tweet.c tweetpts.h
tweetpts_CFLAGS = $(glib_CFLAGS) $(readline_CFLAGS) $(libcurl_CFLAGS) $(oauth_CFLAGS)
tweetpts_LDFLAGS = $(glib_LIBS) $(readline_LIBS) $(libcurl_LIBS) $(oauth_LIBS)