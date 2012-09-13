#include <tweetpts.h>

void cursesapi_call_rest_write(XPANEL *panel,
			       XPANEL *input,
			       gchar *inputfields,
			       gchar *inputstring)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  if(inputstring && strlen(inputstring))
    {
      if(twitterapi_userfields->fields->len)
	fields = twitterapi_get_userfields();
      else
	fields = g_strdup(inputfields);
      g_free(inputfields);

      string = g_strdup(inputstring);
      g_free(inputstring);

      cursesapi_lock(input);
      cursesapi_rest_write(panel, input, fields, string);
    }
}

void cursesapi_get_usersettings(XPANEL *panel)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  if(twitterapi_userfields->fields->len)
     fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_USERSETTINGS_FIELD);

  string = twitterapi_r_usersettings();
  if(string && strlen(string) && string[0] == '{')
    {
      if(fields && strlen(fields) && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);
	  gchar *woeid = NULL;
	  gchar **fieldsv = NULL;

	  cursesapi_push_node(panel, root, fields, FALSE);
	  if(twitterapi_woeid->fields->len == 0)
	    {
	      woeid = jsonapi_get_value(root, "$.trend_location[*].woeid|int");
	      g_mutex_lock(&(twitterapi_woeid->mutex));
	      g_string_assign(twitterapi_woeid->fields, woeid);
	      g_mutex_unlock(&(twitterapi_woeid->mutex));
	    }
	  g_object_unref(parser);
	}
      else
	{
	  top_panel(P(panel));
	  cursesapi_push_string(streampanel, string, 0);
	}
    }
  g_free(fields);
  g_free(string);
}

void cursesapi_get_trendspanel(XPANEL *panel, gchar *woeid)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  g_mutex_lock(&(panel->mutex));

  if(twitterapi_userfields->fields->len)
    fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_TRENDS_FIELD);

  string = twitterapi_r_trends(woeid);
  if(string &&
     strlen(string) &&
     string[0] == '[' &&
     g_strcmp0("raw", fields) != 0)
    {
      JsonParser *parser = jsonapi_parser();
      JsonNode *root = jsonapi_decode(parser, string);
  
      cursesapi_panel_refresh(panel, 1);
      cursesapi_push_node(panel, root, fields, FALSE);
      g_object_unref(parser);
    }
  else
      cursesapi_push_string(panel, string, 0);
  g_free(fields);
  g_free(string);

  g_mutex_unlock(&(panel->mutex));
}

gboolean cursesapi_get_trendspanel_cb(gpointer user_data)
{
  XPANEL *panel = (XPANEL *) user_data;
  cursesapi_get_trendspanel(panel, NULL);
  return(TRUE);
}

void cursesapi_get_trends(gchar *country)
{
  gchar *woeid = NULL;
  gchar **woeidv = NULL;
  guint iter = 0;
  GPtrArray *poolargs = g_ptr_array_new();

  woeid = twitterapi_r_woeid(country);
  if(woeid && strlen(woeid) && g_str_has_prefix(woeid, "url=") != TRUE)
    {
      woeidv = g_strsplit(woeid, "|", 0);
      while(woeidv[iter])
	{
	  gchar *fields = NULL;
	  gchar *string = NULL;
	  gchar *swoeid = g_strstrip(woeidv[iter]);

	  g_mutex_lock(&(twitterapi_woeid->mutex));
	  g_string_assign(twitterapi_woeid->fields, swoeid);
	  g_mutex_unlock(&(twitterapi_woeid->mutex));

	  string = twitterapi_r_trends(swoeid);
	  if(string && strlen(string) && string[0] == '[')
	    fields = g_strdup(T_TRENDS_FIELD);
	  else
	    fields = g_strdup("raw");
	  cursesapi_call_rest_write(streampanel,
				    inputpanel,
				    fields,
				    string);
	  iter++;
	}
      g_strfreev(woeidv);
    }
  else
    {
      gchar *fields = g_strdup("raw");
      gchar *string = g_strdup(woeid);
      cursesapi_call_rest_write(streampanel,
				inputpanel,
				fields,
				string);
    }

  g_free(woeid);
}

void cursesapi_userinput_thread(gpointer data)
{
  GSList *args = (GSList *) data;
  void (*twitterapi_function)(GSList *args) = g_slist_nth_data(args, 0);
  GSList *threadargs = g_slist_copy(g_slist_nth(args, 1));
  twitterapi_function(threadargs);
  g_slist_free(threadargs);
}

void cursesapi_setfields(guint cmdc, gchar **cmdv)
{
  if(cmdc >= 2)
    twitterapi_set_userfields(cmdv[1]);
}

void cursesapi_filter(guint cmdc, gchar **cmdv)
{
  GSList *args = NULL;
  XTHREAD *thread = NULL;
  gchar *track = NULL;
  gchar *follow = NULL;
  gchar *locations = NULL;

  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  follow = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  locations = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

  args = g_slist_append(args, twitterapi_s_stat_filter);
  args = g_slist_append(args, track);
  args = g_slist_append(args, follow);
  args = g_slist_append(args, locations);
  args = g_slist_append(args, cursesapi_write_cb);
  args = g_slist_append(args, streampanel);
  thread = g_new0(XTHREAD, 1);
  thread->thread = g_thread_new("filter",
				(GThreadFunc)
				cursesapi_userinput_thread,
				args);
  thread->args = args;
  g_ptr_array_add(threadarray, thread);
}

void cursesapi_tweetsearch(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *tweetsearch = NULL;
  gchar *q = NULL;
  gchar *geocode = NULL;
  gchar *lang = NULL;
  gchar *locale = NULL;
  gchar *result_type = NULL;
  gchar *count = NULL;
  gchar *until = NULL;
  gchar *since_id = NULL;
  gchar *max_id = NULL;

  q = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  geocode = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);
  lang = (cmdc >= 5 ? g_strdup(cmdv[4]) : NULL);
  locale = (cmdc >= 6 ? g_strdup(cmdv[5]) : NULL);
  result_type = (cmdc >= 7 ? g_strdup(cmdv[6]) : NULL);
  count = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  until = (cmdc >= 8 ? g_strdup(cmdv[7]) : NULL);
  since_id = (cmdc >= 9 ? g_strdup(cmdv[8]) : NULL);
  max_id = (cmdc >= 10 ? g_strdup(cmdv[9]) : NULL);

  tweetsearch = twitterapi_r_tweetsearch(q,
			       geocode,
			       lang,
			       locale,
			       result_type,
			       count,
			       until,
			       since_id,
			       max_id);
  if(tweetsearch && strlen(tweetsearch) && tweetsearch[0] == '{')
    fields = g_strdup(T_TWEETSEARCH_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    tweetsearch);
}

void cursesapi_sample(guint cmdc, gchar **cmdv)
{
  GSList *args = NULL;
  XTHREAD *thread = NULL;

  args = g_slist_append(args, twitterapi_s_stat_sample);
  args = g_slist_append(args, cursesapi_write_cb);
  args = g_slist_append(args, streampanel);
  thread = g_new0(XTHREAD, 1);
  thread->thread = g_thread_new("sample",
				(GThreadFunc)
				cursesapi_userinput_thread,
				args);
  thread->args = args;
  g_ptr_array_add(threadarray, thread);
}

void cursesapi_firehose(guint cmdc, gchar **cmdv)
{
  GSList *args = NULL;
  XTHREAD *thread = NULL;
  gchar *count = NULL;

  count = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);

  args = g_slist_append(args, twitterapi_s_stat_firehose);
  args = g_slist_append(args, count);
  args = g_slist_append(args, cursesapi_write_cb);
  args = g_slist_append(args, streampanel);
  thread = g_new0(XTHREAD, 1);
  thread->thread = g_thread_new("firehose",
				(GThreadFunc)
				cursesapi_userinput_thread,
				args);
  thread->args = args;
  g_ptr_array_add(threadarray, thread);
}

void cursesapi_timeline(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *timeline = NULL;
  gchar *count = NULL;
  gchar *since_id = NULL;
  gchar *max_id = NULL;

  count = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  since_id = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  max_id = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

  timeline = twitterapi_r_timeline(count,
				   since_id,
				   max_id);
  if(timeline && strlen(timeline) && timeline[0] == '[')
    fields = g_strdup(T_TIMELINE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    timeline);
}

void cursesapi_timelinestream(guint cmdc, gchar **cmdv)
{
  GSList *args = NULL;
  XTHREAD *thread = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;

  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  locations = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);

  args = g_slist_append(args, twitterapi_s_timeline);
  args = g_slist_append(args, track);
  args = g_slist_append(args, locations);
  args = g_slist_append(args, cursesapi_write_cb);
  args = g_slist_append(args, streampanel);
  thread = g_new0(XTHREAD, 1);
  thread->thread = g_thread_new("timeline",
				(GThreadFunc)
				cursesapi_userinput_thread,
				args);
  thread->args = args;
  g_ptr_array_add(threadarray, thread);
}

void cursesapi_trends(guint cmdc, gchar **cmdv)
{
  gchar *country = NULL;
  country = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  cursesapi_get_trends(country);
  cursesapi_get_trendspanel(trendspanel, NULL);
}

void cursesapi_start_recording(guint cmdc, gchar **cmdv)
{
  gchar *filename = NULL;
  filename = (cmdc >= 2 ? g_strdup(cmdv[1]) : g_strdup("tweets.json"));
  glibapi_start_recording(filename);
}

void cursesapi_stop_recording(guint cmdc, gchar **cmdv)
{
  glibapi_stop_recording();
}

void cursesapi_playback(guint cmdc, gchar **cmdv)
{
  gchar *fields = g_strdup(T_FILTER_FIELD);
  gchar *filename = NULL;

  filename = (cmdc >= 2 ? g_strdup(cmdv[1]) : g_strdup("tweets.json"));

  cursesapi_lock(streampanel);
  cursesapi_lock(inputpanel);
  cursesapi_panel_refresh(streampanel, 1);
  glibapi_read_tweets(fields, filename, cursesapi_playback_cb);
  cursesapi_unlock(inputpanel);
  cursesapi_unlock(streampanel);
}

void cursesapi_lookup(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *screenname = NULL;
  gchar *user_id = NULL;
  gchar *lookup = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  user_id = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  lookup = twitterapi_r_lookup(screenname, user_id);
  if(lookup && strlen(lookup) && lookup[0] == '[')
    fields = g_strdup(T_LOOKUP_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    lookup);
}

void cursesapi_usersearch(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *q = NULL;
  gchar *page = NULL;
  gchar *count = NULL;
  gchar *usersearch = NULL;

  q = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  page = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  count = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  usersearch = twitterapi_r_usersearch(q, page, count);
  if(usersearch && strlen(usersearch) && usersearch[0] == '[')
    fields = g_strdup(T_USERSEARCH_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    usersearch);
}

void cursesapi_following(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *userid = NULL;
  gchar *screenname = NULL;
  gchar *cursor = NULL;
  gchar *following = NULL;
  gchar **followingv = NULL;
  guint iter = 0;

  userid = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  screenname = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  cursor = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  following = twitterapi_r_following(userid, screenname, cursor);
  if(following && strlen(following))
    fields = g_strdup(T_FOLLOWING_FIELD);
  else
    fields = g_strdup("raw");

  followingv = g_strsplit(following, "\n", 0);
  while(followingv[iter])
    {
      cursesapi_call_rest_write(streampanel,
				inputpanel,
				g_strdup(fields),
				g_strdup(followingv[iter]));
      iter++;
    }
  g_free(fields);
  g_free(following);
  g_strfreev(followingv);
}

void cursesapi_followers(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *userid = NULL;
  gchar *screenname = NULL;
  gchar *cursor = NULL;
  gchar *followers = NULL;
  gchar **followersv = NULL;
  guint iter = 0;

  userid = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  screenname = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  cursor = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  followers = twitterapi_r_followers(userid, screenname, cursor);
  if(followers && strlen(followers))
    fields = g_strdup(T_FOLLOWERS_FIELD);
  else
    fields = g_strdup("raw");

  followersv = g_strsplit(followers, "\n", 0);
  while(followersv[iter])
    {
      cursesapi_call_rest_write(streampanel,
				inputpanel,
				g_strdup(fields),
				g_strdup(followersv[iter]));
      iter++;
    }
  g_free(fields);
  g_free(followers);
  g_strfreev(followersv);
}

void cursesapi_space(void)
{
  gchar *string = NULL;
  cursesapi_toggle_lock(streampanel);
  if(streampanel->lockstatus > 0)
    string = g_strdup_printf("\nstreaming paused(%d)..",
			     streampanel->lockstatus);
  else
    string = g_strdup_printf("\nstreaming resumed(%d)..",
			     streampanel->lockstatus);
  cursesapi_push_string(statuspanel, string, 0);
  g_free(string);
}

void cursesapi_finish(void)
{
  streampanel->stopthread = TRUE;
  cursesapi_push_string(statuspanel, "\nstreaming closed..", 0);
}

void cursesapi_help(void)
{
  GString *help = g_string_new(NULL);

  g_string_append(help, "' ' \t pause/resume streaming\n");
  g_string_append(help, "ctrl+f \t stop streaming\n");
  g_string_append(help, "ctrl+r \t refresh screen\n");
  g_string_append(help, "ctrl+l \t clear screen\n");
  g_string_append(help, "esc \t exit\n");
  g_string_append(help, "h \t this help text\n");
  g_string_append(help, "\n");
  g_string_append(help, "setfields [jsonfield]\n");
  g_string_append(help, "\t json field specification understandable by jsonapi_get_value() function\n");
  g_string_append(help, "\n");
  g_string_append(help, "filter [track] [follow] [locations]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/filter \n");
  g_string_append(help, "\n");
  g_string_append(help, "tweetsearch [q] [geocode] [lang] [locale] [result_type] [count] [until] [since_id] [max_id]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/search/tweets \n");
  g_string_append(help, "\n");
  g_string_append(help, "sample\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/sample \n");
  g_string_append(help, "\n");
  g_string_append(help, "firehose [count]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/firehose \n");
  g_string_append(help, "\n");
  g_string_append(help, "timeline [count] [since_id] [max_id]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/home_timeline \n");
  g_string_append(help, "\n");
  g_string_append(help, "timelinestream [track] [locations]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/user \n");
  g_string_append(help, "\n");
  g_string_append(help, "trends [country]\n");
  g_string_append(help, "\t show what is trending in specific [country]. give 'world' as 'country' to get what is trending globally\n");
  g_string_append(help, "\n");
  g_string_append(help, "startrecord [filename]\n");
  g_string_append(help, "\t start saving all streaming tweets into a json file. Defaults to 'tweets.json' in current directory\n");
  g_string_append(help, "\n");
  g_string_append(help, "stoprecord\n");
  g_string_append(help, "\t stop saving tweets.\n");
  g_string_append(help, "\n");
  g_string_append(help, "playback [filename]\n");
  g_string_append(help, "\t display those stored json tweets in [filename] back in the screen. Defaults to 'tweets.json' file in current directory \n");
  g_string_append(help, "\n");

  cursesapi_call_rest_write(streampanel, inputpanel, g_strdup("raw"), g_strdup(help->str));
  g_string_free(help, TRUE);
}

void cursesapi_userinput(void)
{
  guint iter = 0;

  while(true)
    {
      gchar input;
      gchar cmd[BUFFERSIZE];
      wordexp_t cmdexp;

      g_mutex_lock(&(inputpanel->mutex));
      cursesapi_top(inputpanel);
      noecho();
      flushinp();
      input = wgetch(W(inputpanel));
      g_mutex_unlock(&(inputpanel->mutex));
      echo();

      if(input == 'h')
	{
	  cursesapi_help();
	  continue;
	}
      if(input == ' ')
	{
	  cursesapi_space();
	  continue;
	}
      else if(input == 6) // 6 == Ctrl + f
	{
	  cursesapi_finish();
	  continue;
	}
      else if(input == 18) // 18 == Ctrl + r
	{
	  cursesapi_panel_refresh_all(2);
	  continue;
	}
      else if(input == 12) // 12 == Ctrl + l
	{
	  cursesapi_panel_refresh_all(1);
	  continue;
	}
      else if(input == 27) // 27 == Esc
	{
	  flushinp();
	  break;
	}
      else
	ungetch(input);

      wgetstr(W(inputpanel), cmd);
      flushinp();
      if(wordexp(cmd, &cmdexp, 0) == 0)
	{
	  gchar **cmdv = cmdexp.we_wordv;
	  guint cmdc = cmdexp.we_wordc;

	  if(g_strcmp0("setfields", cmdv[0]) == 0)
	    cursesapi_setfields(cmdc, cmdv);

	  if(g_strcmp0("filter", cmdv[0]) == 0)
	    cursesapi_filter(cmdc, cmdv);

	  if(g_strcmp0("tsearch", cmdv[0]) == 0)
	    cursesapi_tweetsearch(cmdc, cmdv);

	  if(g_strcmp0("sample", cmdv[0]) == 0)
	    cursesapi_sample(cmdc, cmdv);

	  if(g_strcmp0("firehose", cmdv[0]) == 0)
	    cursesapi_firehose(cmdc, cmdv);

	  if(g_strcmp0("timeline", cmdv[0]) == 0)
	    cursesapi_timeline(cmdc, cmdv);

	  if(g_strcmp0("timelinestream", cmdv[0]) == 0)
	    cursesapi_timelinestream(cmdc, cmdv);

	  if(g_strcmp0("trends", cmdv[0]) == 0)
	    cursesapi_trends(cmdc, cmdv);

	  if(g_strcmp0("startrecord", cmdv[0]) == 0)
	    cursesapi_start_recording(cmdc, cmdv);

	  if(g_strcmp0("stoprecord", cmdv[0]) == 0)
	    cursesapi_stop_recording(cmdc, cmdv);

	  if(g_strcmp0("playback", cmdv[0]) == 0)
	    cursesapi_playback(cmdc, cmdv);

	  if(g_strcmp0("lookup", cmdv[0]) == 0)
	    cursesapi_lookup(cmdc, cmdv);

	  if(g_strcmp0("usearch", cmdv[0]) == 0)
	    cursesapi_usersearch(cmdc, cmdv);

	  if(g_strcmp0("following", cmdv[0]) == 0)
	    cursesapi_following(cmdc, cmdv);

	  if(g_strcmp0("followers", cmdv[0]) == 0)
	    cursesapi_followers(cmdc, cmdv);

	  wordfree(&cmdexp);
	}
    }

  while(iter < threadarray->len)
    {
      XTHREAD *thread = g_ptr_array_index(threadarray, iter);
      cursesapi_lock(streampanel);
      g_slist_free(thread->args);
      cursesapi_unlock(streampanel);
      iter++;
    }
  g_ptr_array_free(threadarray, FALSE);
}

