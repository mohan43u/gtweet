#ifndef __TWEETPTS__
#define __TWEETPTS__

#include <stdio.h>
#include <glib-unix.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <tweetpts-oauthapi.h>
#include <tweetpts-curlapi.h>
#include <tweetpts-twitterapi.h>
#include <tweetpts-jsonapi.h>
#include <tweetpts-cursesapi.h>

#define BUFFERSIZE 4098

GMainLoop *tweetpts_mainloop;

#endif
