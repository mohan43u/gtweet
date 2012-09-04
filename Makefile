# Makefile for tweetpts.h

PKGMODS += glib-2.0 ncursesw oauth libcurl panelw json-glib-1.0
INCLUDES += -I"." $(shell pkg-config --cflags $(PKGMODS))
LIBS += $(shell pkg-config --libs $(PKGMODS)) -lreadline
DEFS += -DPACKAGE='"tweetpts"' -DLOCALEDIR='"."'
CFLAGS += -O0 -ggdb
SOURCES := tweetpts.c 		\
	tweetpts-cursesapi.c 	\
	tweetpts-oauthapi.c 	\
	tweetpts-curlapi.c	\
	tweetpts-twitterapi.c	\
	tweetpts-jsonapi.c
HEADERS := tweetpts.h 		\
	tweetpts-cursesapi.h 	\
	tweetpts-oauthapi.h 	\
	tweetpts-curlapi.h	\
	tweetpts-twitterapi.h	\
	tweetpts-jsonapi.h
PROGRAM := tweetpts

CC = gcc
OBJS := $(SOURCES:%.c=%.o)

all : $(PROGRAM)

$(PROGRAM) : $(OBJS) $(HEADERS)
	$(CC) $(LIBS) -o $(PROGRAM) $(OBJS)

$(OBJS) : %.o : %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFS) -o $@ $<

clean:
	rm -fr $(OBJS) $(PROGRAM)

.PHONY: clean