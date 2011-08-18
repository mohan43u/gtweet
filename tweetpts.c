#include <tweetpts.h>

int main(int argc, char *argv[])
{
  GString *access_token = g_string_new(NULL);
  GString *access_secret = g_string_new(NULL);
  GPtrArray *args = g_ptr_array_new_with_free_func(g_free);
  gint iter = 0;
  const gchar *usage = "%s tweetcommand|help [tweetcommandoptions|help]\n"
    "tweetcommand:\n"
    "\thome_timeline\n";
  if(argc < 2 || g_strcmp0("help", argv[1]) == 0)
    {
      g_print(usage, argv[0]);
      exit(1);
    }
  while(iter < argc)
    {
      if(g_strcmp0("",argv[iter]) == 0)
	g_ptr_array_add(args, NULL);
      else
	g_ptr_array_add(args, g_strdup(argv[iter]));
      iter++;
    }

  tweetpts_oauth(access_token, access_secret);
  tweetpts_tweet(access_token, access_secret, args);

  g_string_free(access_token, TRUE);
  g_string_free(access_secret, TRUE);
  g_ptr_array_free(args, TRUE);

  return(0);
}
