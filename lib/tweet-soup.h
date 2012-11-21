#ifndef __TWEET_SOUP_H__
#define __TWEET_SOUP_H__

#include <libsoup/soup.h>

void tweet_soup_init(void);
void tweet_soup_free(void);

void tweet_soup_async(gchar *inputurl,
		      gchar *inputparams,
		      gpointer func,
		      gpointer user_data,
		      gboolean oauth);
gchar* tweet_soup_sync(gchar *url,
		       gchar *params,
		       gboolean oauth);
GString* tweet_soup_gstring_sync(gchar *inputurl,
				 gchar *inputparams,
				 gboolean oauth);
gchar* tweet_soup_sync_media(gchar *inputurl,
			     gchar *inputparams,
			     gboolean oauth,
			     gchar *statuspair,
			     gchar *filenamepair,
			     gchar *replypostidpair);

SoupSession *tweet_soup_session;
SoupLogger *tweet_soup_logger;

#endif /* __TWEET_SOUP_H__ */
