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

#ifndef __TWEET_OAUTH_H__
#define __TWEET_OAUTH_H__

#include <oauth.h>
#include <wordexp.h>

#define YAHOO_APPID "5jjpbPLV34HGmMDspnGKm0CfyR2QDaEaHulep.T59NmM4eA8rdFRPhCSWvVgd8w-"
#define REQ_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define AUTH_URL "https://api.twitter.com/oauth/authorize"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define OAUTHFILE "~/.libgtweet"

gchar* tweet_oauth_expandfilename(gchar *filename);
gchar* tweet_oauth_sign(gchar *consumer_key,
			gchar *consumer_secret,
			gchar *access_key,
			gchar *access_secret,
			gchar *url,
			gchar **params,
			gchar *method);
void tweet_oauth_request_token(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar **request_key,
			       gchar **request_secret);
gchar* tweet_oauth_gen_authurl(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *request_key,
			       gchar *request_secret);
void tweet_oauth_access_token(gchar *pin,
			      gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *request_key,
			      gchar *request_secret,
			      gchar **access_key,
			      gchar **access_secret);
void tweet_oauth_to_file(gchar *appname,
			 gchar *consumer_key,
			 gchar *consumer_secret,
			 gchar *access_key,
			 gchar *access_secret);
gboolean tweet_oauth_from_file(gchar **appname,
			       gchar **consumer_key,
			       gchar **consumer_secret,
			       gchar **access_key,
			       gchar **access_secret);

#endif /* __TWEET_OAUTH_H__ */
