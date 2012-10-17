#Makefile for tweet

CC := gcc
GIRSCANNER := g-ir-scanner
GIRCOMPILER := g-ir-compiler

MODULES := gobject-2.0 oauth libcurl json-glib-1.0 gobject-introspection-1.0
SOURCES := tweet-object.c tweet-curl.c tweet-oauth.c tweet-json.c tweet-twitter.c tweet.c
HEADERS := $(SOURCES:%.c=%.h)

BINARYOBJECTS := $(SOURCES:%.c=%.o)
BINARY := tweet

LIBRARYOBJECTS := $(filter-out tweet.o, $(BINARYOBJECTS))
LIBRARY := libtweet.so

GIRPKGS := $(MODULES:%=--pkg=%)
GIRNAMESPACE := TweetObject
GIRNSVERSION := 0.2
GIRFILE := $(GIRNAMESPACE)-$(GIRNSVERSION).gir

GIRTYPELIBFILE=$(GIRFILE:%.gir=%.typelib)

CFLAGS += $(shell pkg-config --cflags $(MODULES)) -I. -O0 -ggdb -fPIC
LIBS += $(shell pkg-config --libs $(MODULES)) -lreadline

ALL: $(BINARY) $(LIBRARY) $(GIRFILE) $(GIRTYPELIBFILE)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(BINARYOBJECTS) $(HEADERS)
	$(CC) $(LIBS) -o $(BINARY) $(BINARYOBJECTS)

$(LIBRARY): $(LIBRARYOBJECTS) $(HEADERS)
	$(CC) $(LIBS) -shared -o $(LIBRARY) $(LIBRARYOBJECTS)

$(GIRFILE): $(SOURCES) $(HEADERS)
	$(GIRSCANNER) --namespace=$(GIRNAMESPACE) --nsversion=$(GIRNSVERSION) --no-libtool --library=tweet --output=$(GIRFILE) $(GIRPKGS) $(SOURCES) $(HEADERS)

$(GIRTYPELIBFILE): $(GIRFILE)
	$(GIRCOMPILER) --verbose --output=$(GIRTYPELIBFILE) $(GIRFILE)

clean:
	rm -fr $(BINARYOBJECTS) $(BINARY) $(LIBRARY) $(GIRFILE)

.PHONY:
	clean