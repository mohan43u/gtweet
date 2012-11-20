#include <tweet.h>

static void stream_cb(GObject *source_object,
		      GAsyncResult *res,
		      gpointer user_data)
{
  GtweetObject *tweetObject = GTWEET_OBJECT(source_object);
  GCancellable *cancel = G_CANCELLABLE(user_data);
  GString *stream_response = NULL;
  static guint count = 0;

  g_object_get(GTWEET_OBJECT(source_object),
	       "stream_response", &stream_response,
	       NULL);
  g_print("%s\n", stream_response->str);
  count++;
  if(count >= 10)
    g_cancellable_cancel(cancel);
    
  g_string_free(stream_response, TRUE);
}

int main(int argc, char *argv[])
{
  GtweetObject *tweetObject = NULL;
  GCancellable *cancel = NULL;

  g_type_init();

  tweetObject = GTWEET_OBJECT(gtweet_object_new());
  cancel = g_cancellable_new();
  if(!gtweet_object_initkeys(tweetObject))
    {
      gchar *consumer_key = NULL;
      gchar *consumer_secret = NULL;
      gchar *authurl = NULL;
      gchar *cmdline = NULL;
      gchar *pin = NULL;

      
      consumer_key = readline("consumer_key: ");
      consumer_secret = readline("consumer_secret: ");
      authurl = gtweet_object_gen_authurl(tweetObject,
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

  /* 
   * gtweet_object_filterstream(tweetObject,
   * 			     cancel,
   * 			     stream_cb,
   * 			     cancel,
   * 			     "linux,unix",
   * 			     NULL,
   * 			     NULL);
   */
  /* 
   * gtweet_object_hometimeline(tweetObject,
   * 			     NULL,
   * 			     NULL,
   * 			     NULL);
   */
  /* 
   * gtweet_object_updatemedia(tweetObject,
   * 			    "test upload",
   * 			    "/home/mohan/Pictures/Tajmahal.jpg");
   */
  /* 
   * gtweet_object_pbackground(tweetObject,
   * 			    "/home/mohan/Pictures/nature.jpg",
   * 			    NULL,
   * 			    NULL);
   */
		       
  return 0;
}
