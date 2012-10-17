#ifndef __TWEET_CURL_H__
#define __TWEET_CURL_H__

#include <curl/curl.h>

void tweet_curl_http_cb(gchar *inputurl,
			gchar *inputparams,
			gpointer func,
			gpointer userdata,
			gboolean oauth);
gchar* tweet_curl_http(gchar *url,
		       gchar *params,
		       gboolean oauth);
gchar* tweet_curl_http_media(gchar *inputurl,
			     gchar *inputparams,
			     GPtrArray *inputdata,
			     gboolean oauth);

typedef struct _tweet_curl_multipart_t tweet_curl_multipart_t;
struct _tweet_curl_multipart_t
{
  gchar *name;
  gchar *contenttype;
  gchar *contents;
  gchar *filepath;
};

#endif /* __TWEET_CURL_H__ */
