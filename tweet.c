#include <tweetpts.h>

/*
#define HOME_TIMELINE "http://api.twitter.com/1/statuses/home_timeline.xml"
#define UPDATE_URL "http://api.twitter.com/1/statuses/update.xml"
*/

#define HOME_TIMELINE "http://api.twitter.com/1/statuses/home_timeline.json"
#define UPDATE_URL "http://api.twitter.com/1/statuses/update.json"

void tweetpts_home_timeline(GString *access_token, GString *access_secret)
{
  GString *url = g_string_new(HOME_TIMELINE);
  GString *signed_url = g_string_new(NULL);
  GString *content = g_string_new(NULL);
  GString *output = g_string_new(NULL);

  tweetpts_gen_url(url, access_token, access_secret, signed_url, NULL);
  tweetpts_curl(signed_url, NULL, content);
  parsejson(content, output);
  g_print("%s", output->str);

  g_string_free(url, TRUE);
  g_string_free(signed_url, TRUE);
  g_string_free(content, TRUE);
  g_string_free(output, TRUE);
}

void tweetpts_update(GString *access_token, GString *access_secret, const gchar *message)
{
  GString *url = g_string_new(UPDATE_URL);
  GString *signed_url = g_string_new(NULL);
  GString *post = g_string_new(NULL);
  GString *content = g_string_new(NULL);
  GString *output = g_string_new(NULL);

  g_string_append(url, "/?status=");
  g_string_append_uri_escaped(url, message, NULL, FALSE);

  tweetpts_gen_url(url, access_token, access_secret, signed_url, post);
  tweetpts_curl(signed_url, post, content);
  parsejson(content, output);
  g_print("%s", output->str);

  g_string_free(url, TRUE);
  g_string_free(signed_url, TRUE);
  g_string_free(post, TRUE);
  g_string_free(content, TRUE);
  g_string_free(output, TRUE);
}
