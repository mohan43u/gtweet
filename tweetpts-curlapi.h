#ifndef __TWEETPTS_CURLAPI__
#define __TWEETPTS_CURLAPI__

#include <curl/curl.h>

void curlapi_http_cb(gchar *url, gchar *params, GSList *args, gboolean oauth);
gchar* curlapi_http(gchar *url, gchar *params, gboolean oauth);

#endif
