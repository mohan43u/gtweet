#ifndef __TWEET_OAUTH_H__
#define __TWEET_OAUTH_H__

#include <oauth.h>
#include <wordexp.h>
#include <readline/readline.h>

#define YAHOO_APPID "5jjpbPLV34HGmMDspnGKm0CfyR2QDaEaHulep.T59NmM4eA8rdFRPhCSWvVgd8w-"
#define REQ_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define AUTH_URL "https://api.twitter.com/oauth/authorize"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define OAUTHFILE "~/.tweetpts"

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
void tweet_oauth_to_file(gchar *consumer_key,
			 gchar *consumer_secret,
			 gchar *access_key,
			 gchar *access_secret);
gboolean tweet_oauth_from_file(gchar **consumer_key,
			       gchar **consumer_secret,
			       gchar **access_key,
			       gchar **access_secret);

#endif /* __TWEET_OAUTH_H__ */
