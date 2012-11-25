#include <tweet.h>

void tweet_twitter_s_stat_filter(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *track,
				 gchar *follow,
				 gchar *locations,
				 gpointer func,
				 gpointer userdata)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;

  postargs = g_string_new(NULL);
  if(track && strlen(track))
    g_string_append_printf(postargs, "&track=%s", track);
  if(follow && strlen(follow))
    g_string_append_printf(postargs, "&follow=%s", follow);
  if(locations && strlen(locations))
    g_string_append_printf(postargs, "&locations=%s", locations);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_S_STAT_FILTER,
			 &postparams,
			 "POST");
  tweet_soup_async(url, postparams, func, userdata, TRUE);
  g_free(postparams);
  g_free(url);
}

void tweet_twitter_s_stat_sample(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gpointer func,
				 gpointer userdata)
{
  gchar *url = NULL;

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_S_STAT_SAMPLE,
			 NULL,
			 "GET");
  tweet_soup_async(url, NULL, func, userdata, TRUE);
  g_free(url);
}

void tweet_twitter_s_htimeline(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *track,
			       gchar *locations,
			       gpointer func,
			       gpointer userdata)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;

  getargs = g_string_new(NULL);
  if(track && strlen(track))
    g_string_append_printf(getargs, "&track=%s", track);
  if(locations && strlen(locations))
    g_string_append_printf(getargs, "&locations=%s", locations);
  if(getargs->len)
    geturl = g_strdup_printf("%s?with=followings&reply=all%s",
			     T_S_USER,
			     getargs->str);
  else
    geturl = g_strdup_printf("%s?with=followings&reply=all",
			     T_S_USER);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  tweet_soup_async(url, NULL, func, userdata, TRUE);
  g_free(url);
}

gchar* tweet_twitter_r_htimeline(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *count,
				 gchar *since_id,
				 gchar *max_id,
				 gchar *trimuser)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(count && strlen(count))
    g_string_append_printf(getargs, "&count=%s", count);
  if(since_id && strlen(since_id))
    g_string_append_printf(getargs, "&since_id=%s", since_id);
  if(max_id && strlen(max_id))
    g_string_append_printf(getargs, "&max_id=%s", max_id);
  if(trimuser && strlen(trimuser))
    g_string_append_printf(getargs, "&trim_user=%s", trimuser);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_HTIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_HTIMELINE);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_utimeline(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *since_id,
				 gchar *count,
				 gchar *max_id)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    g_string_append_printf(getargs, "&user_id=%s", userid);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(since_id && strlen(since_id))
    g_string_append_printf(getargs, "&since_id=%s", since_id);
  if(count && strlen(count))
    g_string_append_printf(getargs, "&count=%s", count);
  if(max_id && strlen(max_id))
    g_string_append_printf(getargs, "&max_id=%s", max_id);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_UTIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_UTIMELINE);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_usersettings(gchar *consumer_key,
				    gchar *consumer_secret,
				    gchar *access_key,
				    gchar *access_secret)
{
  gchar *url = NULL;
  gchar *result = NULL;

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_ACCOUNTSETTINGS,
			 NULL,
			 "GET");
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_woeid(gchar *countryname)
{
  gchar *url = NULL;
  gchar *result = NULL;
  gchar *woeid = NULL;

  if(countryname)
    {
      url = g_strdup_printf("%s/places.q(%s)?appid=%s&format=json",
			    Y_R_WOEID,
			    countryname,
			    YAHOO_APPID);
      result = tweet_soup_sync(url, NULL, FALSE);
    }
  return result;
}

gchar* tweet_twitter_r_trends(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *woeid)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  gchar *result = NULL;

  geturl = g_strdup_printf(T_R_TRENDS, woeid);
  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_tweetsearch(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *q,
				   gchar *geocode,
				   gchar *lang,
				   gchar *locale,
				   gchar *result_type,
				   gchar *count,
				   gchar *until,
				   gchar *since_id,
				   gchar *max_id)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(q && strlen(q))
    g_string_append_printf(getargs, "&q=%s", q);
  if(geocode && strlen(geocode))
    g_string_append_printf(getargs, "&geocode=%s", geocode);
  if(lang && strlen(lang))
    g_string_append_printf(getargs, "&lang=%s", lang);
  if(locale && strlen(locale))
    g_string_append_printf(getargs, "&locale=%s", locale);
  if(result_type && strlen(result_type))
    g_string_append_printf(getargs, "&result_type=%s", result_type);
  if(count && strlen(count))
    g_string_append_printf(getargs, "&count=%s", count);
  if(until && strlen(until))
    g_string_append_printf(getargs, "&until=%s", until);
  if(since_id && strlen(since_id))
    g_string_append_printf(getargs, "&since_id=%s", since_id);
  if(max_id && strlen(max_id))
    g_string_append_printf(getargs, "&max_id=%s", max_id);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_TWEETSEARCH, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_TWEETSEARCH);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_lookup(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *screenname,
			      gchar *user_id)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    g_string_append_printf(postargs, "&screen_name=%s", screenname);
  if(user_id && strlen(user_id))
    g_string_append_printf(postargs, "&user_id=%s", user_id);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_LOOKUP,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  g_free(postparams);
  return result;
}

gchar* tweet_twitter_r_show(gchar *consumer_key,
			    gchar *consumer_secret,
			    gchar *access_key,
			    gchar *access_secret,
			    gchar *user_id,
			    gchar *screenname)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(user_id && strlen(user_id))
    g_string_append_printf(getargs, "&user_id=%s", user_id);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_SHOW, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_SHOW);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_usersearch(gchar *consumer_key,
				  gchar *consumer_secret,
				  gchar *access_key,
				  gchar *access_secret,
				  gchar *q,
				  gchar *page,
				  gchar *count)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(q && strlen(q))
    g_string_append_printf(getargs, "&q=%s", q);
  if(page && strlen(page))
    g_string_append_printf(getargs, "&page=%s", page);
  if(count && strlen(count))
    g_string_append_printf(getargs, "&count=%s", count);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_USERSEARCH, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_USERSEARCH);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_following(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    g_string_append_printf(getargs, "&user_id=%s", userid);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(cursor && strlen(cursor))
    g_string_append_printf(getargs, "&cursor=%s", cursor);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s&stringify_ids=true", T_R_FOLLOWING, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWING);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_followers(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    g_string_append_printf(getargs, "&user_id=%s", userid);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(cursor && strlen(cursor))
    g_string_append_printf(getargs, "&cursor=%s", cursor);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s&stringify_ids=true", T_R_FOLLOWERS, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWERS);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_updatemedia(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *status,
				   gchar *filepath,
				   gchar *replypostid)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  gchar *result = NULL;
  gchar *statuspair = NULL;
  gchar *filepathpair = NULL;
  gchar *replypostidpair = NULL;

  statuspair = g_strdup_printf("status:%s", status);
  filepathpair = g_strdup_printf("media[]:%s", filepath);
  replypostidpair = g_strdup_printf("in_reply_to_post_id:%s", replypostid);

  posturl = g_strdup(T_R_UPDATEMEDIA);
  postparams = g_strdup("");
  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 posturl,
			 &postparams,
			 "POST");
  g_free(posturl);
  result = tweet_soup_sync_media(url,
				 postparams, 
				 TRUE,
				 statuspair,
				 filepathpair,
				 replypostidpair);
  g_free(url);
  g_free(postparams);
  g_free(statuspair);
  g_free(filepathpair);
  g_free(replypostidpair);
  return result;
}

gchar* tweet_twitter_r_update(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *status,
			      gchar *replypostid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(status && strlen(status))
    {
      gchar *estatus = NULL;
      estatus = g_uri_escape_string(status, NULL, TRUE);
      g_string_append_printf(postargs, "&status=%s", estatus);
      g_free(estatus);
    }
  if(replypostid && strlen(replypostid))
    g_string_append_printf(postargs, "&in_reply_to_status_id=%s", replypostid);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_UPDATE,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_retweet(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *postid)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  gchar *result = NULL;

  if(postid && strlen(postid))
    posturl = g_strdup_printf(T_R_RETWEET, postid);
  else
    return NULL;

  postparams = g_strdup("");
  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 posturl,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(postparams);
  g_free(url);
  g_free(posturl);
  return result;
}

gchar* tweet_twitter_r_showstatus(gchar *consumer_key,
				  gchar *consumer_secret,
				  gchar *access_key,
				  gchar *access_secret,
				  gchar *postid)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  gchar *result = NULL;

  if(postid && strlen(postid))
    geturl = g_strdup_printf(T_R_SHOWSTATUS, postid);
  else
    return NULL;

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  g_free(geturl);
  return result;
}

gchar* tweet_twitter_r_destroy(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *postid)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  gchar *result = NULL;

  if(postid && strlen(postid))
    posturl = g_strdup_printf(T_R_DESTROY, postid);
  else
    return NULL;

  postparams = g_strdup("");
  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 posturl,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_follow(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *screenname,
			      gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    g_string_append_printf(postargs, "&screen_name=%s", screenname);
  if(userid && strlen(userid))
    g_string_append_printf(postargs, "&user_id=%s", userid);
  if(postargs->len)
    postparams = g_strdup_printf("follow=true%s", postargs->str);
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_FOLLOW,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_unfollow(gchar *consumer_key,
				gchar *consumer_secret,
				gchar *access_key,
				gchar *access_secret,
				gchar *screenname,
				gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    g_string_append_printf(postargs, "&screen_name=%s", screenname);
  if(userid && strlen(userid))
    g_string_append_printf(postargs, "&user_id=%s", userid);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_UNFOLLOW,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_blocklist(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(cursor && strlen(cursor))
    g_string_append_printf(getargs, "&cursor=%s", cursor);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_BLOCKLIST, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_BLOCKLIST);
  g_string_free(getargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 geturl,
			 NULL,
			 "GET");
  g_free(geturl);
  result = tweet_soup_sync(url, NULL, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_block(gchar *consumer_key,
			     gchar *consumer_secret,
			     gchar *access_key,
			     gchar *access_secret,
			     gchar *screenname,
			     gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    g_string_append_printf(postargs, "&screen_name=%s", screenname);
  if(userid && strlen(userid))
    g_string_append_printf(postargs, "&user_id=%s", userid);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_BLOCK,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_unblock(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *screenname,
			       gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    g_string_append_printf(postargs, "&screen_name=%s", screenname);
  if(userid && strlen(userid))
    g_string_append_printf(postargs, "&user_id=%s", userid);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_UNBLOCK,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_profile(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *name,
			       gchar *purl,
			       gchar *location,
			       gchar *description)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(name && strlen(name))
    g_string_append_printf(postargs, "&name=%s", name);
  if(purl && strlen(purl))
    g_string_append_printf(postargs, "&purl=%s", purl);
  if(location && strlen(location))
    g_string_append_printf(postargs, "&location=%s", location);
  if(description && strlen(description))
    g_string_append_printf(postargs, "&description=%s", description);
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_PROFILE,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}

gchar* tweet_twitter_r_pbackground(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *filepath,
				   gchar *tile,
				   gchar *use)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;
  gchar *filepathpair = NULL;

  filepathpair = g_strdup_printf("image:%s", filepath);

  posturl = g_strdup(T_R_PBACKGROUND);
  postparams = g_strdup("");
  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 posturl,
			 &postparams,
			 "POST");
  g_free(posturl);
  result = tweet_soup_sync_media(url,
				 postparams, 
				 TRUE,
				 NULL,
				 filepathpair,
				 NULL);
  if(result && strlen(result) && result[0] == '{')
    {
      if(use && strlen(use))
	{
	  postargs = g_string_new(NULL);
	  g_string_append_printf(postargs, "&use=%s", use);
	  g_free(use);
	}
      else
	postargs = g_string_new("use=1");
      if(tile && strlen(tile))
	{
	  g_string_append_printf(postargs, "&tile=%s", tile);
	  g_free(tile);
	}
      postparams = g_strdup(postargs->str);

      g_free(url);
      g_free(postparams);
      g_free(result);
      posturl = g_strdup(T_R_PBACKGROUND);
      postparams = g_strdup(postargs->str);
      url = tweet_oauth_sign(consumer_key,
			     consumer_secret,
			     access_key,
			     access_secret,
			     posturl,
			     &postparams,
			     "POST");
      g_free(posturl);
      result = tweet_soup_sync(url, postparams, TRUE);
    }
  g_free(url);
  g_free(postparams);
  g_free(filepathpair);

  return result;
}

gchar* tweet_twitter_r_pimage(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *filepath)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(filepath && strlen(filepath))
    {
      gchar *filecontent = NULL;
      gchar *fullpath = NULL;
      gchar *encodedcontent = NULL;
      gsize length = 0; 

      fullpath = tweet_oauth_expandfilename(filepath);
      g_file_get_contents(fullpath, &filecontent, &length, NULL);
      encodedcontent = g_base64_encode(filecontent, length);
      g_string_append_printf(postargs, "&image=%s", encodedcontent);

      g_free(encodedcontent);
      g_free(filecontent);
      g_free(fullpath);
    }
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = tweet_oauth_sign(consumer_key,
			 consumer_secret,
			 access_key,
			 access_secret,
			 T_R_PIMAGE,
			 &postparams,
			 "POST");
  result = tweet_soup_sync(url, postparams, TRUE);
  g_free(url);
  return result;
}
