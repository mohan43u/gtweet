#include <tweet.h>

int main(int argc, char *argv[])
{
  GtweetObject *tweetObject = NULL;

  g_type_init();

  tweetObject = GTWEET_OBJECT(gtweet_object_new());
  if(!gtweet_object_initkeys(tweetObject))
    {
      gchar *consumer_key = NULL;
      gchar *consumer_secret = NULL;
      gchar *authurl = NULL;
      gchar *cmdline = NULL;
      gchar *pin = NULL;

      
      consumer_key = readline("consumer_key: ");
      consumer_secret = readline("consumer_secret: ");
      authurl = gtweet_object_authurl(tweetObject,
				      consumer_key,
				      consumer_secret);
      cmdline = g_strdup_printf("xdg-open '%s'", authurl);
      if(g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL))
	{
	  pin = readline("pin: ");
	  gtweet_object_auth(tweetObject, pin);
	}

      g_free(consumer_key);
      g_free(consumer_secret);
      g_free(authurl);
      g_free(cmdline);
      g_free(pin);
    }

  return 0;
}
