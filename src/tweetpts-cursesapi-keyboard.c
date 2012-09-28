#include <tweetpts.h>

static void cursesapi_call_rest_write(cursesapi_panel_t *panel,
				      cursesapi_panel_t *input,
				      gchar *fields, gchar *string);
static void cursesapi_get_trends(gchar *country);
static void cursesapi_userinput_thread(gpointer data);
static void cursesapi_help(void);
static void cursesapi_space(void);
static void cursesapi_finish(void);
static void cursesapi_start_recording(guint cmdc, gchar **cmdv);
static void cursesapi_stop_recording(guint cmdc, gchar **cmdv);
static void cursesapi_playback(guint cmdc, gchar **cmdv);
static void cursesapi_tweetsearch(guint cmdc, gchar **cmdv);
static void cursesapi_filter_thread(gpointer data);
static void cursesapi_filter(guint cmdc, gchar **cmdv);
static void cursesapi_sample_thread(gpointer data);
static void cursesapi_sample(guint cmdc, gchar **cmdv);
static void cursesapi_htimelinestream_thread(gpointer data);
static void cursesapi_htimelinestream(guint cmdc, gchar **cmdv);
static void cursesapi_htimeline(guint cmdc, gchar **cmdv);
static void cursesapi_utimeline(guint cmdc, gchar **cmdv);
static void cursesapi_trends(guint cmdc, gchar **cmdv);
static void cursesapi_lookup(guint cmdc, gchar **cmdv);
static void cursesapi_usersearch(guint cmdc, gchar **cmdv);
static void cursesapi_following(guint cmdc, gchar **cmdv);
static void cursesapi_followers(guint cmdc, gchar **cmdv);
static void cursesapi_updatemedia(guint cmdc, gchar **cmdv);
static void cursesapi_update(guint cmdc, gchar **cmdv);
static void cursesapi_retweet(guint cmdc, gchar **cmdv);
static void cursesapi_destroy(guint cmdc, gchar **cmdv);
static void cursesapi_follow(guint cmdc, gchar **cmdv);
static void cursesapi_unfollow(guint cmdc, gchar **cmdv);
static void cursesapi_blocklist(guint cmdc, gchar **cmdv);
static void cursesapi_block(guint cmdc, gchar **cmdv);
static void cursesapi_unblock(guint cmdc, gchar **cmdv);
static void cursesapi_profile(guint cmdc, gchar **cmdv);
static void cursesapi_pbackground(guint cmdc, gchar **cmdv);
static void cursesapi_pimage(guint cmdc, gchar **cmdv);

static void cursesapi_call_rest_write(cursesapi_panel_t *panel,
				      cursesapi_panel_t *input,
				      gchar *fields, gchar *string)
{
  if(string && strlen(string))
    {
      cursesapi_lock(input);
      cursesapi_rest_write(panel, input, fields, string);
    }
}

void cursesapi_get_usersettings(cursesapi_panel_t *panel)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  fields = g_strdup(J_USERSETTINGS_FIELD);
  string = twitterapi_r_usersettings();
  if(string && strlen(string) && string[0] == '{')
    {
      JsonParser *parser = jsonapi_parser();
      JsonNode *root = jsonapi_decode(parser, string);
      gchar *woeid = NULL;

      cursesapi_push_node(panel, root, fields, FALSE);
      if(cursesapi_woeid == NULL)
	{
	  woeid = jsonapi_get_value(root, "$.trend_location[*].woeid|int");
	  g_mutex_lock(&(trendspanel->mutex));
	  cursesapi_woeid = g_strdup(woeid);
	  g_mutex_unlock(&(trendspanel->mutex));
	}
      g_object_unref(parser);
    }
  else
    cursesapi_push_string(panel, string, 0);

  g_free(fields);
  g_free(string);
}

void cursesapi_get_trendspanel(cursesapi_panel_t *panel, gchar *woeid)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  fields = g_strdup(J_TRENDS_FIELD);
  if(woeid == NULL)
    {
      if(cursesapi_woeid)
	{
	  g_mutex_lock(&(trendspanel->mutex));
	  string = twitterapi_r_trends(cursesapi_woeid);
	  g_mutex_unlock(&(trendspanel->mutex));
	}
      else
	string = twitterapi_r_trends("1");
    }
  else
    string = twitterapi_r_trends(woeid);

  if(string && strlen(string) && string[0] == '[')
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
}

gboolean cursesapi_get_trendspanel_cb(gpointer user_data)
{
  cursesapi_panel_t *panel = (cursesapi_panel_t *) user_data;
  cursesapi_get_trendspanel(panel, NULL);
  return(TRUE);
}

static void cursesapi_get_trends(gchar *country)
{
  gchar *woeid = NULL;
  guint iter = 0;
  GPtrArray *poolargs = g_ptr_array_new();

  woeid = twitterapi_r_woeid(country);
  if(woeid && strlen(woeid) && g_str_has_prefix(woeid, "url=") != TRUE)
    {
      gchar **woeidv = NULL;

      woeidv = g_strsplit(woeid, "|", 0);
      while(woeidv[iter])
	{
	  gchar *fields = NULL;
	  gchar *string = NULL;
	  gchar *swoeid = g_strstrip(woeidv[iter]);

	  g_mutex_lock(&(trendspanel->mutex));
	  if(cursesapi_woeid)
	    {
	      g_free(cursesapi_woeid);
	      cursesapi_woeid = g_strdup(swoeid);
	    }
	  else
	    cursesapi_woeid = g_strdup(swoeid);
	  g_mutex_unlock(&(trendspanel->mutex));

	  string = twitterapi_r_trends(swoeid);
	  if(string && strlen(string) && string[0] == '[')
	    fields = g_strdup(J_TRENDS_FIELD);
	  else
	    fields = g_strdup("raw");
	  cursesapi_call_rest_write(streampanel,
				    inputpanel,
				    fields,
				    string);
	  g_free(fields);
	  g_free(string);

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
      g_free(fields);
      g_free(string);
    }

  g_free(woeid);
}

static void cursesapi_space(void)
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

static void cursesapi_finish(void)
{
  streampanel->stopthread = TRUE;
  cursesapi_push_string(statuspanel, "\nstreaming closed..", 0);
}

static void cursesapi_start_recording(guint cmdc, gchar **cmdv)
{
  gchar *filename = NULL;
  filename = (cmdc >= 2 ? g_strdup(cmdv[1]) : g_strdup("tweets.json"));
  glibapi_start_recording(filename);
}

static void cursesapi_stop_recording(guint cmdc, gchar **cmdv)
{
  glibapi_stop_recording();
}

static void cursesapi_playback(guint cmdc, gchar **cmdv)
{
  gchar *fields = g_strdup(J_FILTER_FIELD);
  gchar *filename = NULL;

  filename = (cmdc >= 2 ? g_strdup(cmdv[1]) : g_strdup("tweets.json"));

  cursesapi_lock(streampanel);
  cursesapi_lock(inputpanel);
  cursesapi_panel_refresh(streampanel, 1);
  glibapi_read_tweets(fields, filename, cursesapi_playback_cb);
  cursesapi_unlock(inputpanel);
  cursesapi_unlock(streampanel);
}

static void cursesapi_tweetsearch(guint cmdc, gchar **cmdv)
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
  result_type = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  count = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);
  geocode = (cmdc >= 5 ? g_strdup(cmdv[4]) : NULL);
  lang = (cmdc >= 6 ? g_strdup(cmdv[5]) : NULL);
  locale = (cmdc >= 7 ? g_strdup(cmdv[6]) : NULL);
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
    fields = g_strdup(J_TWEETSEARCH_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    tweetsearch);

  g_free(q);
  g_free(result_type);
  g_free(count);
  g_free(geocode);
  g_free(lang);
  g_free(locale);
  g_free(until);
  g_free(since_id);
  g_free(max_id);
  g_free(fields);
  g_free(tweetsearch);
}

static void cursesapi_filter_thread(gpointer data)
{
  GPtrArray *args = NULL;
  gchar *track = NULL;
  gchar *follow = NULL;
  gchar *locations = NULL;
  gchar *fields = NULL;
  GPtrArray *userdata = NULL;

  args = data;
  track = g_ptr_array_index(args, 0);
  follow = g_ptr_array_index(args, 1);
  locations = g_ptr_array_index(args, 2);

  fields = g_strdup(J_FILTER_FIELD);
  userdata = g_ptr_array_new();
  g_ptr_array_add(userdata, streampanel);
  g_ptr_array_add(userdata, fields);

  twitterapi_s_stat_filter(track,
			   follow,
			   locations,
			   cursesapi_write_cb,
			   userdata);

  g_free(track);
  g_free(follow);
  g_free(locations);
  g_free(fields);
  g_ptr_array_free(userdata, FALSE);
  g_ptr_array_free(args, FALSE);
}

static void cursesapi_filter(guint cmdc, gchar **cmdv)
{
  GPtrArray *args = NULL;
  GThread *thread = NULL;
  gchar *track = NULL;
  gchar *follow = NULL;
  gchar *locations = NULL;

  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  follow = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  locations = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

  args = g_ptr_array_new();
  g_ptr_array_add(args, track);
  g_ptr_array_add(args, follow);
  g_ptr_array_add(args, locations);

  thread = g_thread_new("filter",
			(GThreadFunc) cursesapi_filter_thread,
			args);
}

static void cursesapi_sample_thread(gpointer data)
{
  gchar *fields = NULL;
  GPtrArray *userdata = NULL;

  fields = g_strdup(J_SAMPLE_FIELD);
  userdata = g_ptr_array_new();
  g_ptr_array_add(userdata, streampanel);
  g_ptr_array_add(userdata, fields);

  twitterapi_s_stat_sample(cursesapi_write_cb, userdata);

  g_free(fields);
  g_ptr_array_free(userdata, FALSE);
}

static void cursesapi_sample(guint cmdc, gchar **cmdv)
{
  GThread *thread = NULL;
  gchar *fields = NULL;

  thread = g_thread_new("sample",
			(GThreadFunc) cursesapi_sample_thread,
			NULL);
}

static void cursesapi_htimelinestream_thread(gpointer data)
{
  GPtrArray *args = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;
  gchar *fields = NULL;
  GPtrArray *userdata = NULL;

  args = data;
  track = g_ptr_array_index(args, 0);
  locations = g_ptr_array_index(args, 1);

  fields = g_strdup(J_HTIMELINE_FIELD);
  userdata = g_ptr_array_new();
  g_ptr_array_add(userdata, streampanel);
  g_ptr_array_add(userdata, fields);

  twitterapi_s_htimeline(track,
			 locations,
			 cursesapi_write_cb,
			 userdata);

  g_free(track);
  g_free(locations);
  g_free(fields);
  g_ptr_array_free(userdata, FALSE);
  g_ptr_array_free(args, FALSE);
}

static void cursesapi_htimelinestream(guint cmdc, gchar **cmdv)
{
  GPtrArray *args = NULL;
  GThread *thread = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;
  gchar *fields = NULL;

  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  locations = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);

  args = g_ptr_array_new();
  g_ptr_array_add(args, track);
  g_ptr_array_add(args, locations);

  thread = g_thread_new("timeline",
			(GThreadFunc) cursesapi_htimelinestream_thread,
			args);
}

static void cursesapi_htimeline(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *timeline = NULL;
  gchar *count = NULL;
  gchar *since_id = NULL;
  gchar *max_id = NULL;

  count = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  since_id = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  max_id = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

  timeline = twitterapi_r_htimeline(count,
				   since_id,
				   max_id);
  if(timeline && strlen(timeline) && timeline[0] == '[')
    fields = g_strdup(J_HTIMELINE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    timeline);

  g_free(count);
  g_free(since_id);
  g_free(max_id);
  g_free(fields);
  g_free(timeline);
}

static void cursesapi_utimeline(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *timeline = NULL;
  gchar *userid = NULL;
  gchar *screenname = NULL;
  gchar *since_id = NULL;
  gchar *count = NULL;
  gchar *max_id = NULL;

  screenname = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  count = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
  userid = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);
  since_id = (cmdc >= 5 ? g_strdup(cmdv[4]) : NULL);
  max_id = (cmdc >= 6 ? g_strdup(cmdv[5]) : NULL);

  timeline = twitterapi_r_utimeline(userid,
				    screenname,
				    since_id,
				    count,
				    max_id);
  if(timeline && strlen(timeline) && timeline[0] == '[')
    fields = g_strdup(J_UTIMELINE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    timeline);

  g_free(screenname);
  g_free(count);
  g_free(userid);
  g_free(since_id);
  g_free(max_id);
  g_free(fields);
  g_free(timeline);
}

static void cursesapi_trends(guint cmdc, gchar **cmdv)
{
  gchar *country = NULL;
  country = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  cursesapi_get_trends(country);
  cursesapi_get_trendspanel(trendspanel, NULL);
  g_free(country);
}

static void cursesapi_lookup(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *screenname = NULL;
  gchar *user_id = NULL;
  gchar *lookup = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  user_id = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  lookup = twitterapi_r_lookup(screenname, user_id);
  if(lookup && strlen(lookup) && lookup[0] == '[')
    fields = g_strdup(J_LOOKUP_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    lookup);

  g_free(screenname);
  g_free(user_id);
  g_free(fields);
  g_free(lookup);
}

static void cursesapi_usersearch(guint cmdc, gchar **cmdv)
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
    fields = g_strdup(J_USERSEARCH_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    usersearch);

  g_free(fields);
  g_free(q);
  g_free(page);
  g_free(count);
  g_free(usersearch);
}

static void cursesapi_following(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *userid = NULL;
  gchar *screenname = NULL;
  gchar *cursor = NULL;
  gchar *following = NULL;
  gchar **followingv = NULL;
  guint iter = 0;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  cursor = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  following = twitterapi_r_following(userid, screenname, cursor);
  if(following && strlen(following) && following[0] == '[')
    {
      fields = g_strdup(J_FOLLOWING_FIELD);
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

  g_free(userid);
  g_free(screenname);
  g_free(cursor);
}

static void cursesapi_followers(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *userid = NULL;
  gchar *screenname = NULL;
  gchar *cursor = NULL;
  gchar *followers = NULL;
  gchar **followersv = NULL;
  guint iter = 0;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  cursor = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  followers = twitterapi_r_followers(userid, screenname, cursor);
  if(followers && strlen(followers) && followers[0] == '[')
    {
      fields = g_strdup(J_FOLLOWERS_FIELD);
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

  g_free(userid);
  g_free(screenname);
  g_free(cursor);
}

static void cursesapi_updatemedia(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *update = NULL;
  gchar *status = NULL;
  gchar *filepath = NULL;

  status = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
  filepath = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);

  update = twitterapi_r_updatemedia(status,
				    filepath);
  if(update && strlen(update) && update[0] == '{')
    fields = g_strdup(J_UPDATEMEDIA_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    update);

  g_free(fields);
  g_free(update);
  g_free(status);
  g_free(filepath);
}

static void cursesapi_update(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *update = NULL;
  gchar *status = NULL;
  gchar *replypostid = NULL;

  status = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  replypostid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  update = twitterapi_r_update(status, replypostid);

  if(update && strlen(update) && update[0] == '{')
    fields = g_strdup(J_UPDATE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    update);

  g_free(fields);
  g_free(update);
  g_free(status);
  g_free(replypostid);
}

static void cursesapi_retweet(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *retweet = NULL;
  gchar *postid = NULL;

  postid = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);

  retweet = twitterapi_r_retweet(postid);

  if(retweet && strlen(retweet) && retweet[0] == '{')
    fields = g_strdup(J_RETWEET_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    retweet);

  g_free(fields);
  g_free(retweet);
  g_free(postid);
}

static void cursesapi_destroy(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *destroy = NULL;
  gchar *postid = NULL;

  postid = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);

  destroy = twitterapi_r_destroy(postid);

  if(destroy && strlen(destroy) && destroy[0] == '{')
    fields = g_strdup(J_DESTROY_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    destroy);

  g_free(fields);
  g_free(destroy);
  g_free(postid);
}

static void cursesapi_follow(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *follow = NULL;
  gchar *screenname = NULL;
  gchar *userid = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  follow = twitterapi_r_follow(screenname, userid);

  if(follow && strlen(follow) && follow[0] == '{')
    fields = g_strdup(J_FOLLOW_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    follow);

  g_free(fields);
  g_free(follow);
  g_free(screenname);
  g_free(userid);
}

static void cursesapi_unfollow(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *unfollow = NULL;
  gchar *screenname = NULL;
  gchar *userid = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  unfollow = twitterapi_r_unfollow(screenname, userid);

  if(unfollow && strlen(unfollow) && unfollow[0] == '{')
    fields = g_strdup(J_UNFOLLOW_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    unfollow);

  g_free(fields);
  g_free(unfollow);
  g_free(screenname);
  g_free(userid);
}

static void cursesapi_blocklist(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *blocklist = NULL;
  gchar *cursor = NULL;

  cursor = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);

  blocklist = twitterapi_r_blocklist(cursor);
  if(blocklist && strlen(blocklist) && blocklist[0] == '[')
    fields = g_strdup(J_BLOCKLIST_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    blocklist);

  g_free(fields);
  g_free(blocklist);
  g_free(cursor);
}

static void cursesapi_block(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *block = NULL;
  gchar *screenname = NULL;
  gchar *userid = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  block = twitterapi_r_block(screenname, userid);

  if(block && strlen(block) && block[0] == '{')
    fields = g_strdup(J_BLOCK_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    block);

  g_free(fields);
  g_free(block);
  g_free(screenname);
  g_free(userid);
}

static void cursesapi_unblock(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *unblock = NULL;
  gchar *screenname = NULL;
  gchar *userid = NULL;

  screenname = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  userid = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);

  unblock = twitterapi_r_unblock(screenname, userid);

  if(unblock && strlen(unblock) && unblock[0] == '{')
    fields = g_strdup(J_UNBLOCK_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    unblock);

  g_free(fields);
  g_free(unblock);
  g_free(screenname);
  g_free(userid);
}

static void cursesapi_profile(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *profile = NULL;
  gchar *name = NULL;
  gchar *purl = NULL;
  gchar *location = NULL;
  gchar *description = NULL;

  name = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  purl = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  location = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);
  description = (cmdc >= 5? g_strdup(cmdv[4]) : NULL);

  profile = twitterapi_r_profile(name,
				 purl,
				 location,
				 description);

  if(profile && strlen(profile) && profile[0] == '{')
    fields = g_strdup(J_PROFILE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    profile);

  g_free(fields);
  g_free(profile);
  g_free(name);
  g_free(purl);
  g_free(location);
  g_free(description);
}

static void cursesapi_pbackground(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *pbackground = NULL;
  gchar *filepath = NULL;
  gchar *tile = NULL;
  gchar *use = NULL;

  filepath = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);
  use = (cmdc >= 3? g_strdup(cmdv[2]) : NULL);
  tile = (cmdc >= 4? g_strdup(cmdv[3]) : NULL);

  pbackground = twitterapi_r_pbackground(filepath, tile, use);

  if(pbackground && strlen(pbackground) && pbackground[0] == '{')
    fields = g_strdup(J_PBACKGROUND_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    pbackground);

  g_free(fields);
  g_free(pbackground);
  g_free(filepath);
  g_free(tile);
  g_free(use);
}

static void cursesapi_pimage(guint cmdc, gchar **cmdv)
{
  gchar *fields = NULL;
  gchar *pimage = NULL;
  gchar *filepath = NULL;

  filepath = (cmdc >= 2? g_strdup(cmdv[1]) : NULL);

  pimage = twitterapi_r_pimage(filepath);

  if(pimage && strlen(pimage) && pimage[0] == '{')
    fields = g_strdup(J_PIMAGE_FIELD);
  else
    fields = g_strdup("raw");
  cursesapi_call_rest_write(streampanel,
			    inputpanel,
			    fields,
			    pimage);

  g_free(fields);
  g_free(pimage);
  g_free(filepath);
}

static void cursesapi_help(void)
{
  GString *help = g_string_new(NULL);

  g_string_append(help, "' ' \t pause/resume streaming\n");
  g_string_append(help, "Ctrl-f \t stop streaming\n");
  g_string_append(help, "Ctrl-r \t refresh screen\n");
  g_string_append(help, "Ctrl-l \t clear screen\n");
  g_string_append(help, "Ctrl-h \t this help text\n");
  g_string_append(help, "\n");
  g_string_append(help, "tsearch [q] [result_type] [count] [geocode] [lang] [locale] [until] [since_id] [max_id]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/search/tweets \n");
  g_string_append(help, "\n");
  g_string_append(help, "filter [track] [follow] [locations]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/filter \n");
  g_string_append(help, "\n");
  g_string_append(help, "sample\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/sample \n");
  g_string_append(help, "\n");
  g_string_append(help, "htimelinestream [track] [locations]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/user \n");
  g_string_append(help, "\n");
  g_string_append(help, "htimeline [count] [since_id] [max_id]\n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/home_timeline \n");
  g_string_append(help, "\n");
  g_string_append(help, "utimeline [screenname] [count] [since_id] [max_id] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/statuses/user_timeline \n");
  g_string_append(help, "\n");
  g_string_append(help, "trends [country]\n");
  g_string_append(help, "\t show what is trending in specific [country]. give 'world' as 'country' to get what is trending globally\n");
  g_string_append(help, "\n");
  g_string_append(help, "lookup [screenname] [user_id] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/users/lookup \n");
  g_string_append(help, "\n");
  g_string_append(help, "usearch [q] [page] [count] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/users/search \n");
  g_string_append(help, "\n");
  g_string_append(help, "following [screenname] [userid] [cursor] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/friends/ids \n");
  g_string_append(help, "\n");
  g_string_append(help, "followers [screenname] [userid] [cursor] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/followers/ids \n");
  g_string_append(help, "\n");
  g_string_append(help, "updatemedia [status] [filepath] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/update_with_media \n");
  g_string_append(help, "\n");
  g_string_append(help, "update [status] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/update \n");
  g_string_append(help, "\n");
  g_string_append(help, "retweet [postid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/retweet/%3Aid \n");
  g_string_append(help, "\n");
  g_string_append(help, "destroy [postid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/statuses/destroy/%3Aid \n");
  g_string_append(help, "\n");
  g_string_append(help, "follow [screenname] [userid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/friendships/create \n");
  g_string_append(help, "\n");
  g_string_append(help, "unfollow [screenname] [userid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/friendships/destroy \n");
  g_string_append(help, "\n");
  g_string_append(help, "blocklist [cursor] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/get/blocks/list \n");
  g_string_append(help, "\n");
  g_string_append(help, "block [screenname] [userid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/blocks/create \n");
  g_string_append(help, "\n");
  g_string_append(help, "unblock [screenname] [userid] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/blocks/destroy \n");
  g_string_append(help, "\n");
  g_string_append(help, "profile [name] [url] [location] [description] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/account/update_profile \n");
  g_string_append(help, "\n");
  g_string_append(help, "pbackground [filepath] [use] [tile] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/account/update_profile_background_image \n");
  g_string_append(help, "\n");
  g_string_append(help, "pimage [filepath] \n");
  g_string_append(help, "\t https://dev.twitter.com/docs/api/1.1/post/account/update_profile_image \n");
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
  g_string_append(help, "exit\n");
  g_string_append(help, "\t exit from this application\n");
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
      gchar *cmd;
      wordexp_t cmdexp;

      g_mutex_lock(&(inputpanel->mutex));
      cursesapi_top(inputpanel);
      noecho();
      flushinp();
      input = wgetch(W(inputpanel));
      g_mutex_unlock(&(inputpanel->mutex));
      echo();

      if(input == ' ')
	{
	  cursesapi_space();
	  continue;
	}
      else if(input == 8) // 8 == Ctrl + h
	{
	  cursesapi_help();
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
      else
	ungetch(input);

      cmd = readlineapi_wgetstr();
      flushinp();
      if(wordexp(cmd, &cmdexp, 0) == 0)
	{
	  gchar **cmdv = cmdexp.we_wordv;
	  guint cmdc = cmdexp.we_wordc;

	  if(g_strcmp0("tsearch", cmdv[0]) == 0)
	    cursesapi_tweetsearch(cmdc, cmdv);

	  if(g_strcmp0("filter", cmdv[0]) == 0)
	    cursesapi_filter(cmdc, cmdv);

	  if(g_strcmp0("sample", cmdv[0]) == 0)
	    cursesapi_sample(cmdc, cmdv);

	  if(g_strcmp0("htimelinestream", cmdv[0]) == 0)
	    cursesapi_htimelinestream(cmdc, cmdv);

	  if(g_strcmp0("htimeline", cmdv[0]) == 0)
	    cursesapi_htimeline(cmdc, cmdv);

	  if(g_strcmp0("utimeline", cmdv[0]) == 0)
	    cursesapi_utimeline(cmdc, cmdv);

	  if(g_strcmp0("trends", cmdv[0]) == 0)
	    cursesapi_trends(cmdc, cmdv);

	  if(g_strcmp0("lookup", cmdv[0]) == 0)
	    cursesapi_lookup(cmdc, cmdv);

	  if(g_strcmp0("usearch", cmdv[0]) == 0)
	    cursesapi_usersearch(cmdc, cmdv);

	  if(g_strcmp0("following", cmdv[0]) == 0)
	    cursesapi_following(cmdc, cmdv);

	  if(g_strcmp0("followers", cmdv[0]) == 0)
	    cursesapi_followers(cmdc, cmdv);

	  if(g_strcmp0("updatemedia", cmdv[0]) == 0)
	    cursesapi_updatemedia(cmdc, cmdv);

	  if(g_strcmp0("update", cmdv[0]) == 0)
	    cursesapi_update(cmdc, cmdv);

	  if(g_strcmp0("retweet", cmdv[0]) == 0)
	    cursesapi_retweet(cmdc, cmdv);

	  if(g_strcmp0("destroy", cmdv[0]) == 0)
	    cursesapi_destroy(cmdc, cmdv);

	  if(g_strcmp0("follow", cmdv[0]) == 0)
	    cursesapi_follow(cmdc, cmdv);

	  if(g_strcmp0("unfollow", cmdv[0]) == 0)
	    cursesapi_unfollow(cmdc, cmdv);

	  if(g_strcmp0("blocklist", cmdv[0]) == 0)
	    cursesapi_blocklist(cmdc, cmdv);

	  if(g_strcmp0("block", cmdv[0]) == 0)
	    cursesapi_block(cmdc, cmdv);

	  if(g_strcmp0("unblock", cmdv[0]) == 0)
	    cursesapi_unblock(cmdc, cmdv);

	  if(g_strcmp0("profile", cmdv[0]) == 0)
	    cursesapi_profile(cmdc, cmdv);

	  if(g_strcmp0("pbackground", cmdv[0]) == 0)
	    cursesapi_pbackground(cmdc, cmdv);

	  if(g_strcmp0("pimage", cmdv[0]) == 0)
	    cursesapi_pimage(cmdc, cmdv);

	  if(g_strcmp0("startrecord", cmdv[0]) == 0)
	    cursesapi_start_recording(cmdc, cmdv);

	  if(g_strcmp0("stoprecord", cmdv[0]) == 0)
	    cursesapi_stop_recording(cmdc, cmdv);

	  if(g_strcmp0("playback", cmdv[0]) == 0)
	    cursesapi_playback(cmdc, cmdv);

	  if(g_strcmp0("exit", cmdv[0]) == 0)
	    {
	      flushinp();
	      break;
	    }

	  wordfree(&cmdexp);
	}
      g_free(cmd);
    }
}
