#include <tweetpts.h>

static void tweetpts_init(void)
{
  twitterapi_init();
  cursesapi_init();
  jsonapi_init();
}

static void tweetpts_free(void)
{
  jsonapi_free();
  cursesapi_free();
  twitterapi_free();
}

int main(int argc, char *argv[])
{
  tweetpts_init();
  cursesapi_userinput();
  tweetpts_free();
  return(0);
}
