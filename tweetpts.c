#include <tweetpts.h>

static gpointer mainloop_init(gpointer data)
{
  GMainLoop *mainloop = (GMainLoop *) data;
  if(mainloop)
    g_main_loop_run(mainloop);
}

static void tweetpts_init(void)
{
  GThread *mainloopthread = NULL;

  oauthapi_init();
  twitterapi_init();
  cursesapi_init();
  jsonapi_init();

  tweetpts_mainloop = g_main_loop_new(NULL, TRUE);
  mainloopthread = g_thread_new("mainloopthread",
				mainloop_init,
				tweetpts_mainloop);
}

static void tweetpts_free(void)
{
  jsonapi_free();
  cursesapi_free();
  twitterapi_free();
  oauthapi_free();

  g_main_loop_quit(tweetpts_mainloop);
}

int main(int argc, char *argv[])
{
  GPtrArray *plist = NULL;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
  tweetpts_init();

  plist = cursesapi_create_baselayout();
  cursesapi_userinput(plist);
  cursesapi_destroy_baselayout(plist);

  tweetpts_free();
  return(0);
}
