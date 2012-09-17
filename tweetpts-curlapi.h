#ifndef __TWEETPTS_CURLAPI__
#define __TWEETPTS_CURLAPI__

#include <curl/curl.h>

void curlapi_http_cb(gchar *url,
		     gchar *params,
		     GSList *args,
		     gboolean oauth);
gchar* curlapi_http(gchar *url,
		    gchar *params,
		    gboolean oauth);
gchar* curlapi_http_media(gchar *inputurl,
			  gchar *inputparams,
			  GPtrArray *inputdata,
			  gboolean oauth);

typedef struct
{
  gchar *name;
  gchar *contenttype;
  gchar *contents;
  gchar *filepath;
} curlapi_multipart_t;

#endif
