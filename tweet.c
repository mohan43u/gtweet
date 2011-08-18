#include <tweetpts.h>

/* API URLs must end with '/' so, be careful while adding */
#define HOME_TIMELINE "http://api.twitter.com/1/statuses/home_timeline.json/"

static void home_timeline(GString *access_token, GString *access_secret, GPtrArray *args)
{
  GString *url = g_string_new(HOME_TIMELINE);
  GString *signed_url = g_string_new(NULL);
  GString *parameters = g_string_new(NULL);
  GString *get = g_string_new(NULL);
  GString *content = g_string_new(NULL);
  GString *output = g_string_new(NULL);

  if(g_strcmp0("help", g_ptr_array_index(args, 2)) == 0)
    {
      g_printerr("[usage]\n"
		 "\t%s home_timeline \n"
		 "\t\t[count {should be < 200, eg: 30}] \n"
		 "\t\t[since_id {returns tweets > since_id, eg: 12345}] \n"
		 "\t\t[max_id {returns tweets < max_id, eg: 54321}] \n"
		 "\t\t[page {page number to retrive, eg: 3}] \n"
		 "\t\t[trim_user {returns only status and userid, values: true/false}] \n"
		 "\t\t[include_rts {returns native tweets, values: true/false}] \n"
		 "\t\t[include_entities {more metadata, values: true/false}] \n"
		 "\t\t[exclude_replies {prevent reply tweets, values: true/false}] \n"
		 "\t\t[contributor_details {include screen_name, values: true/false}] \n"
		 "[example]\n"
		 "\tBelow command will retrive 2 status messages from 5th page of your\n"
		 "\thome_timeline.(count=2, page=5)\n"
		 "\t\t$ %s home_timeline '2' '' '' '5'\n",
		 g_ptr_array_index(args, 0),
		 g_ptr_array_index(args, 0));
      return;
    }

  if(g_ptr_array_index(args, 2) != NULL)
    g_string_append_printf(parameters, "&count=%s", g_ptr_array_index(args, 2));
  if(g_ptr_array_index(args, 3) != NULL)
    g_string_append_printf(parameters, "&since_id=%s", g_ptr_array_index(args, 3));
  if(g_ptr_array_index(args, 4) != NULL)
    g_string_append_printf(parameters, "&max_id=%s", g_ptr_array_index(args, 4));
  if(g_ptr_array_index(args, 5) != NULL)
    g_string_append_printf(parameters, "&page=%s", g_ptr_array_index(args, 5));
  if(g_ptr_array_index(args, 6) != NULL)
    g_string_append_printf(parameters, "&trim_user=%s", g_ptr_array_index(args, 6));
  if(g_ptr_array_index(args, 7) != NULL)
    g_string_append_printf(parameters, "&include_rts=%s", g_ptr_array_index(args, 7));
  if(g_ptr_array_index(args, 8) != NULL)
    g_string_append_printf(parameters, "&include_entities=%s", g_ptr_array_index(args, 8));
  if(g_ptr_array_index(args, 9) != NULL)
    g_string_append_printf(parameters, "&exclude_replies=%s", g_ptr_array_index(args, 9));
  if(g_ptr_array_index(args, 10) != NULL)
    g_string_append_printf(parameters, "&contributor_details=%s", g_ptr_array_index(args, 10));

  g_string_append(url, parameters->str);
  tweetpts_gen_url(url, get, "GET", access_token, access_secret, signed_url);
  g_string_append_printf(signed_url, "?%s", get->str);
  tweetpts_curl(signed_url, NULL, content);
  parsejson(content, output);
  g_print("%s", output->str);

  g_string_free(url, TRUE);
  g_string_free(signed_url, TRUE);
  g_string_free(parameters, TRUE);
  g_string_free(get, TRUE);
  g_string_free(content, TRUE);
  g_string_free(output, TRUE);
}

void tweetpts_tweet(GString *access_token, GString *access_secret, GPtrArray *args)
{
  if(g_strcmp0("home_timeline", g_ptr_array_index(args, 1)) == 0)
    home_timeline(access_token, access_secret, args);
}
