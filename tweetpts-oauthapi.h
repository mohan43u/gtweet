#ifndef __TWEETPTS_OAUTHAPI__
#define __TWEETPTS_OAUTHAPI__

#include <oauth.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wordexp.h>

#define CONSUMER_KEY "DjbukDI8FmGP3vPBNstK4g"
#define CONSUMER_SECRET "2wOCuS2MRZGwHB20lp1v9Q2qDpmtJTiGSz5aKkC2s"
#define YAHOO_APPID "5jjpbPLV34HGmMDspnGKm0CfyR2QDaEaHulep.T59NmM4eA8rdFRPhCSWvVgd8w-"

#define REQ_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define AUTH_URL "https://api.twitter.com/oauth/authorize"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"

#define CMDLINE_FMT "xdg-open '%s&%s&%s'"
#define OAUTHFILE expandfilename("~/.tweetpts")

gchar *request_key_pair;
gchar *request_secret_pair;
gchar *request_key;
gchar *request_secret;
gchar *access_key_pair;
gchar *access_secret_pair;
gchar *access_key;
gchar *access_secret;
gchar *pin;

gchar* oauthapi_sign(gchar *url, gchar **params);
void oauthapi_request_token(void);
void oauthapi_authenticate(void);
void oauthapi_access_token(void);
gchar* expandfilename(gchar *filename);
void oauthapi_init(void);
void oauthapi_access_token_from_file(void);
void oauthapi_access_token_to_file(void);
void oauthapi_free(void);

#endif
