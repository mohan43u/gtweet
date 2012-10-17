#include <tweet.h>

static tweet_oauth_init(void)
{
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *authurl = NULL;
  gchar *cmdline = NULL;
  gchar *pin = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  if(!tweet_oauth_access_token_from_file(&access_key, &access_secret))
    {
      tweet_oauth_request_token(&request_key, &request_secret);
      authurl = tweet_oauth_gen_authurl(request_key, request_secret);
      if(authurl)
	{
	  cmdline = g_strdup_printf("xdg-open '%s'", authurl);
	  if(g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL))
	    {
	      pin = readline("PIN: ");
	      tweet_oauth_access_token(pin,
				       request_key,
				       request_secret,
				       &access_key,
				       &access_secret);
	    }
	  tweet_oauth_access_token_to_file(access_key, access_secret);
	}
    }

  g_free(request_key);
  g_free(request_secret);
  g_free(authurl);
  g_free(cmdline);
  g_free(pin);
  g_free(access_key);
  g_free(access_secret);
}

int main(int argc, char **argv)
{
  TweetObject *tweetObject = NULL;

  g_type_init();

  tweetObject = g_object_new(TWEET_TYPE_OBJECT, NULL);
  tweet_oauth_init();
  g_object_unref(tweetObject);
  return(0);
}
