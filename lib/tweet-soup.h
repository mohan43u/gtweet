/*
 * gtweet - GLib/GObject wrapper for twitter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
