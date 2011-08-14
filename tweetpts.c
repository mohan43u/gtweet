#include <tweetpts.h>

int main(int argc, char *argv[])
{
  GString *access_token = g_string_new(NULL);
  GString *access_secret = g_string_new(NULL);
  gchar *statusmessage = NULL;
  const gchar *usage = "[usage] tweetpts <1=home_timeline/2=update> [statusmessage (when arg1 = 2)]\n";
  if(argc < 2)
    {
      g_print(usage);
      exit(1);
    }
  else if(g_strcmp0("2", argv[1]) == 0 && argc < 3)
    {
      g_print(usage);
      exit(1);
    }
  else
    statusmessage = g_strdup(argv[2]);

  tweetpts_oauth(access_token, access_secret);
  if(g_strcmp0("1", argv[1]) == 0)
    tweetpts_home_timeline(access_token, access_secret);
  else
    tweetpts_update(access_token, access_secret, statusmessage);
  
  g_string_free(access_token, TRUE);
  g_string_free(access_secret, TRUE);
  g_free(statusmessage);

  return(0);
}
