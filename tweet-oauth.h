#ifndef __TWEET_OAUTH_H__
#define __TWEET_OAUTH_H__

#include <oauth.h>
#include <wordexp.h>
#include <readline/readline.h>

#define CONSUMER_KEY "EpQdWugEGn0L8sSeDyNxQ"
#define CONSUMER_SECRET "e21wC3jBLlpDtjf4pKrD0m3br7LgPjwMFukhq31Vg"
#define YAHOO_APPID "5jjpbPLV34HGmMDspnGKm0CfyR2QDaEaHulep.T59NmM4eA8rdFRPhCSWvVgd8w-"
#define REQ_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define AUTH_URL "https://api.twitter.com/oauth/authorize"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"
#define OAUTHFILE "~/.tweetpts"

gchar* tweet_oauth_expandfilename(gchar *filename);
gchar* tweet_oauth_sign(gchar *access_key,
			gchar *access_secret,
			gchar *url,
			gchar **params,
			gchar *method);
void tweet_oauth_request_token(gchar **request_key,
			       gchar **request_secret);
gchar* tweet_oauth_gen_authurl(gchar *request_key,
			       gchar *request_secret);
void tweet_oauth_access_token(gchar *pin,
			      gchar *request_key,
			      gchar *request_secret,
			      gchar **access_key,
			      gchar **access_secret);
void tweet_oauth_access_token_to_file(gchar *access_key,
				      gchar *access_secret);
gboolean tweet_oauth_access_token_from_file(gchar **access_key,
					    gchar **access_secret);

#endif /* __TWEET_OAUTH_H__ */
