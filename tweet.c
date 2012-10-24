#include <tweet.h>
#include <girepository.h>

static gboolean sample_stream_cb(gchar *string,
				 gpointer userdata)
{
  g_print("%s\n", string);
}

int main(int argc, char **argv)
{
  GTweetObject *tweetObject = NULL;
  GOptionContext *ctx = NULL;
  GError *error = NULL;

  g_type_init();

  ctx = g_option_context_new(NULL);
  g_option_context_add_group(ctx, g_irepository_get_option_group());

  if(!g_option_context_parse(ctx, &argc, &argv, NULL))
    {
      g_printerr("tweet: %s\n", error->message);
      exit(EXIT_FAILURE);
    }

  tweetObject = G_TWEETOBJECT(g_tweet_object_new());
  if(!g_tweet_object_initkeys(tweetObject))
    {
      gchar *authurl = NULL;
      gchar *cmdline = NULL;
      gchar *pin = NULL;

      authurl = g_tweet_object_authurl(tweetObject);
      cmdline = g_strdup_printf("xdg-open '%s'", authurl);
      if(g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL))
	{
	  pin = readline("PIN: ");
	  g_tweet_object_auth(tweetObject, pin);
	}

      g_free(authurl);
      g_free(cmdline);
      g_free(pin);
    }

  g_tweet_object_samplestream(tweetObject,
			      sample_stream_cb,
			      NULL);

  g_object_unref(tweetObject);
  return(0);
}
