#Makefile for tweet

CC := gcc
GIRSCANNER := g-ir-scanner
GIRCOMPILER := g-ir-compiler

MODULES := gobject-2.0 oauth libcurl json-glib-1.0 gobject-introspection-1.0
SOURCES := tweet-object.c tweet-curl.c tweet-oauth.c tweet-json.c tweet-twitter.c tweet.c
HEADERS := $(SOURCES:%.c=%.h)
CFLAGS += $(shell pkg-config --cflags $(MODULES)) -I. -O0 -ggdb -fPIC
LIBS += $(shell pkg-config --libs $(MODULES)) -lreadline

BINARYOBJECTS := $(SOURCES:%.c=%.o)
BINARY := tweet
LIBRARYOBJECTS := $(filter-out tweet.o, $(BINARYOBJECTS))
LIBRARY := libtweet.so

GIRPKGS := $(MODULES:%=--pkg=%)
GIRNAMESPACE := GTweetObject
GIRNSVERSION := 0.2
IDPREFIX := G
GIRFILE := $(GIRNAMESPACE)-$(GIRNSVERSION).gir
GIRTYPELIBFILE=$(GIRFILE:%.gir=%.typelib)

ALL: $(BINARY) $(LIBRARY) $(GIRFILE) $(GIRTYPELIBFILE)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(BINARYOBJECTS) $(HEADERS)
	$(CC) $(LIBS) -o $(BINARY) $(BINARYOBJECTS)

$(LIBRARY): $(LIBRARYOBJECTS) $(HEADERS)
	$(CC) $(LIBS) -shared -o $(LIBRARY) $(LIBRARYOBJECTS)

$(GIRFILE): $(SOURCES) $(HEADERS)
	$(GIRSCANNER) \
		--include=GObject-2.0 \
		--identifier-prefix=$(IDPREFIX) \
		--namespace=$(GIRNAMESPACE) \
		--nsversion=$(GIRNSVERSION) \
		--no-libtool \
		--library=tweet \
		--output=$(GIRFILE) \
		$(GIRPKGS) \
		$(SOURCES) $(HEADERS)

$(GIRTYPELIBFILE): $(GIRFILE)
	$(GIRCOMPILER) --output=$(GIRTYPELIBFILE) $(GIRFILE)

clean:
	rm -fr $(BINARYOBJECTS) $(BINARY) $(LIBRARY) $(GIRFILE) $(GIRTYPELIBFILE)

.PHONY:
	clean