#include <tweetpts.h>

void cursesapi_get_usersettings(XPANEL *panel)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  if(twitterapi_userfields->fields->len)
     fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_USERSETTINGS_FIELD);

  string = twitterapi_get_usersettings();
  if(string && strlen(string))
    {
      if(string[0] == '{' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);
	  gchar *woeid = NULL;

	  cursesapi_push_element(panel, root, fields);
	  if(twitterapi_woeid->fields->len == 0)
	    {
	      woeid = jsonapi_get_value(root, ".trend_location[*].woeid|int");
	      g_mutex_lock(&(twitterapi_woeid->mutex));
	      g_string_assign(twitterapi_woeid->fields, woeid);
	      g_mutex_unlock(&(twitterapi_woeid->mutex));
	    }
	  g_object_unref(parser);
	}
      else
	{
	  top_panel(P(panel));
	  cursesapi_push_string(panel, string, 0);
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

  string = twitterapi_get_trends(woeid);
  if(string && strlen(string))
    {
      if(string[0] == '[' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);

	  cursesapi_panel_refresh(panel, 1);
	  cursesapi_push_array(panel, root, fields, FALSE);
	  g_object_unref(parser);
	}
      else
	{
	  top_panel(P(panel));
	  cursesapi_push_string(panel, string, 0);
	}
    }
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

void cursesapi_get_trends(XPANEL *totop,
				 XPANEL *tobottom,
				 XPANEL *input,
				 gchar *country)
{
  gchar *woeid = NULL;
  gchar **woeidv = NULL;
  guint iter = 0;
  GPtrArray *poolargs = g_ptr_array_new();

  cursesapi_set_pause(input);
  woeid = twitterapi_get_woeid(country);
  if(woeid && strlen(woeid) && g_str_has_prefix(woeid, "url=") != TRUE)
    {
      woeidv = g_strsplit(woeid, "|", 0);
      while(woeidv[iter])
	{
	  gchar *fields = NULL;
	  gchar *string = NULL;
	  gchar *swoeid = g_strstrip(woeidv[iter]);

	  if(twitterapi_userfields->fields->len)
	    fields = twitterapi_get_userfields();
	  else
	    fields = g_strdup(T_TRENDS_FIELD);

	  g_mutex_lock(&(twitterapi_woeid->mutex));
	  g_string_assign(twitterapi_woeid->fields, swoeid);
	  g_mutex_unlock(&(twitterapi_woeid->mutex));

	  string = twitterapi_get_trends(swoeid);
	  if(string && strlen(string))
	    {
	      if(string[0] != '[')
		{
		  g_free(fields);
		  fields = g_strdup("raw");
		}
	      g_ptr_array_add(poolargs, tobottom);
	      g_ptr_array_add(poolargs, input);
	      g_ptr_array_add(poolargs, fields);
	      g_ptr_array_add(poolargs, string);
	      g_thread_pool_push(totop->pool, poolargs, NULL);
	    }
	  iter++;
	}
      g_strfreev(woeidv);
    }
  else
    {
      gchar *fields = g_strdup("raw");
      gchar *string = g_strdup(woeid);
      g_ptr_array_add(poolargs, tobottom);
      g_ptr_array_add(poolargs, input);
      g_ptr_array_add(poolargs, fields);
      g_ptr_array_add(poolargs, string);
      g_thread_pool_push(totop->pool, poolargs, NULL);
    }

  g_free(woeid);
}

void cursesapi_userinput_thread(gpointer data)
{
  GSList *args = (GSList *) data;
  void (*twitterapi_function)(GSList *args) = (g_slist_nth(args, 0))->data;
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
  GSList *args = NULL;
  XTHREAD *thread = NULL;
  gchar *timeline = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;
  gchar *count = NULL;

  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  locations = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  count = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

  timeline = twitterapi_r_timeline(count);
  if(timeline && strlen(timeline))
    {
      GPtrArray *poolargs = g_ptr_array_new();
      gchar *fields = NULL;
      gchar *string = NULL;

      if(twitterapi_userfields->fields->len)
	fields = twitterapi_get_userfields();
      else
	fields = g_strdup(T_TIMELINE_FIELD);
      string = g_strdup(timeline);
      g_free(timeline);

      g_ptr_array_add(poolargs, streampanel);
      g_ptr_array_add(poolargs, inputpanel);
      g_ptr_array_add(poolargs, fields);
      g_ptr_array_add(poolargs, string);
      g_thread_pool_push(restpanel->pool, poolargs, NULL);
      cursesapi_push_string(statuspanel,
			    "\nstreaming paused..",
			    0);
    }
  if(g_strcmp0("timelinestream", cmdv[0]) == 0)
    {
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
}

void cursesapi_trends(guint cmdc, gchar **cmdv)
{
  gchar *country = NULL;
  country = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  cursesapi_get_trends(restpanel,
		       streampanel,
		       inputpanel,
		       country);
  cursesapi_get_trendspanel(trendspanel, NULL);
  cursesapi_push_string(statuspanel, "\nstreaming paused..", 0);
}

void cursesapi_usersettings(guint cmdc, gchar **cmdv)
{
  cursesapi_get_usersettings(userpanel);
}

void cursesapi_space(void)
{
  cursesapi_toggle_pause(streampanel);
  if(streampanel->lockstatus == TRUE)
    cursesapi_push_string(statuspanel, "\nstreaming paused..", 0);
  else
    cursesapi_push_string(statuspanel, "\nstreaming resumed..", 0);
}

void cursesapi_finish(void)
{
  streampanel->stopthread = TRUE;
  cursesapi_push_string(statuspanel, "\nstreaming closed..", 0);
}

void cursesapi_help(void)
{
  GString *help = g_string_new(NULL);
  GPtrArray *poolargs = g_ptr_array_new();
  gchar *string = NULL;

  g_string_append(help, "h \t this help message\n");
  g_string_append(help, "' ' \t pause/resume streaming\n");
  g_string_append(help, "ctrl+f \t finish streaming\n");
  g_string_append(help, "ctrl+r \t refresh streen\n");
  g_string_append(help, "ctrl+l \t clear streen\n");
  g_string_append(help, "ESC \t exit\n\n");
  g_string_append(help, "setfields [fieldformat|raw|default]\n");
  g_string_append(help, "\t to set json fields for next apicall\n");
  g_string_append(help, "\t fieldformat - string which can be understandable\n");
  g_string_append(help, "\t  \t by jsonapi_get_value() function\n");
  g_string_append(help, "\t raw - direct json text\n");
  g_string_append(help, "\t default - default fields for that apicall\n\n");
  g_string_append(help, "filter [track] [follow] [location]\n");
  g_string_append(help, "\t to get tweets about some string, or person, or location\n");
  g_string_append(help, "\t track - string to track\n");
  g_string_append(help, "\t follow - person's uid to follow\n");
  g_string_append(help, "\t location - location defailt see https://dev.twitter.com/docs/streaming-apis/parameters#locations\n\n");
  g_string_append(help, "sample\n");
  g_string_append(help, "\t streaming tweet sample, be careful, your screen will roll heavily\n\n");
  g_string_append(help, "timeline|timelinestream [track] [location] [count]\n");
  g_string_append(help, "\t to see current user's home timeline, if timelinestream, update if any of his friend tweets\n");
  g_string_append(help, "\t track - see filter command\n");
  g_string_append(help, "\t location - see filter command\n");
  g_string_append(help, "\t count - restrict number of tweets to show in timeline\n\n");
  g_string_append(help, "trends [country]\n");
  g_string_append(help, "\t see whats trending\n");
  g_string_append(help, "\t country - trends for this country, it also updates the country track, if not give, it defaults to current user's country\n");

  string = g_strdup(help->str);
  g_ptr_array_add(poolargs, streampanel);
  g_ptr_array_add(poolargs, inputpanel);
  g_ptr_array_add(poolargs, NULL);
  g_ptr_array_add(poolargs, string);

  cursesapi_set_pause(inputpanel);
  g_thread_pool_push(restpanel->pool, poolargs, NULL);

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

	  if(g_strcmp0("sample", cmdv[0]) == 0)
	    cursesapi_sample(cmdc, cmdv);

	  if(g_strcmp0("firehose", cmdv[0]) == 0)
	    cursesapi_firehose(cmdc, cmdv);

	  if(g_strcmp0("timelinestream", cmdv[0]) == 0 ||
	     g_strcmp0("timeline", cmdv[0]) == 0)
	    cursesapi_timeline(cmdc, cmdv);

	  if(g_strcmp0("trends", cmdv[0]) == 0)
	    cursesapi_trends(cmdc, cmdv);

	  wordfree(&cmdexp);
	}
    }

  while(iter < threadarray->len)
    {
      XTHREAD *thread = g_ptr_array_index(threadarray, iter);
      cursesapi_set_pause(streampanel);
      cursesapi_set_pause(restpanel);
      g_slist_free(thread->args);
      cursesapi_remove_pause(restpanel);
      cursesapi_remove_pause(streampanel);
      iter++;
    }
  g_ptr_array_free(threadarray, FALSE);
}

