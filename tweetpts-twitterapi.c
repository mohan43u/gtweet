#include <tweetpts.h>

void twitterapi_init(void)
{
  twitterapi_userfields = g_new0(XFIELD, 1);
  twitterapi_userfields->fields = g_string_new(NULL);
  g_mutex_init(&(twitterapi_userfields->mutex));

  twitterapi_woeid = g_new0(XFIELD, 1);
  twitterapi_woeid->fields = g_string_new(NULL);
  g_mutex_init(&(twitterapi_woeid->mutex));
}

void twitterapi_free(void)
{
  g_string_free(twitterapi_userfields->fields, TRUE);
  g_mutex_clear(&(twitterapi_userfields->mutex));
  g_free(twitterapi_userfields);

  g_string_free(twitterapi_woeid->fields, TRUE);
  g_mutex_clear(&(twitterapi_woeid->mutex));
  g_free(twitterapi_woeid);
}

void twitterapi_set_userfields(gchar *fields)
{
  if(fields)
    {
      g_mutex_lock(&(twitterapi_userfields->mutex));
      if(fields && g_strcmp0("default", fields) == 0)
	g_string_set_size(twitterapi_userfields->fields, 0);
      else
	g_string_assign(twitterapi_userfields->fields, fields);
      g_mutex_unlock(&(twitterapi_userfields->mutex));
    }
}

gchar* twitterapi_get_userfields(void)
{
  gchar *string = NULL;

  if(twitterapi_userfields->fields->len)
    {
      g_mutex_lock(&(twitterapi_userfields->mutex));
      string = g_strdup(twitterapi_userfields->fields->str);
      g_string_set_size(twitterapi_userfields->fields, 0);
      g_mutex_unlock(&(twitterapi_userfields->mutex));
    }
  return(string);
}

void twitterapi_s_stat_filter(GSList *args)
{
  gchar *url = NULL;
  GString *postargs = NULL;
  gchar *searchpattern = NULL;
  gchar *fields = NULL;
  GSList *threadargs = NULL;
  gchar *track = NULL;
  gchar *follow = NULL;
  gchar *locations = NULL;

  track = (g_slist_nth(args, 0))->data;
  follow = (g_slist_nth(args, 1))->data;
  locations = (g_slist_nth(args, 2))->data;

  postargs = g_string_new(NULL);
  if(track && strlen(track))
    {
      g_string_append_printf(postargs, "&track=%s", track);
      g_free(track);
    }
  if(follow && strlen(follow))
    {
      g_string_append_printf(postargs, "&follow=%s", follow);
      g_free(follow);
    }
  if(locations && strlen(locations))
    {
      g_string_append_printf(postargs, "&locations=%s", locations);
      g_free(locations);
    }
  if(postargs->len)
    searchpattern = g_strdup(&(postargs->str[1]));
  else
    searchpattern = g_strdup("");
  g_string_free(postargs, TRUE);

  if(twitterapi_userfields->fields->len)
    fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_FILTER_FIELD);

  url = oauthapi_sign(T_S_STAT_FILTER, &searchpattern);
  threadargs = g_slist_copy(g_slist_nth(args, 3));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, searchpattern, threadargs);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(searchpattern);
  g_free(url);
}

void twitterapi_s_stat_sample(GSList *args)
{
  gchar *url = NULL;
  gchar *fields = NULL;
  GSList *threadargs = NULL;

  if(twitterapi_userfields->fields->len)
    fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_SAMPLE_FIELD);

  url = oauthapi_sign(T_S_STAT_SAMPLE, NULL);
  threadargs = g_slist_copy(g_slist_nth(args, 0));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, NULL, threadargs);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(url);
}

void twitterapi_s_stat_firehose(GSList *args)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *fields = NULL;
  GSList *threadargs = NULL;
  gchar *count = NULL;
  
  count = (g_slist_nth(args, 0))->data;

  getargs = g_string_new(NULL);
  if(count && strlen(count))
    {
      g_string_append_printf(getargs, "&count=%s", count);
      g_free(count);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_S_STAT_FIREHOSE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_S_STAT_FIREHOSE);
  g_string_free(getargs, TRUE);

  if(twitterapi_userfields->fields->len)
    fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_FIREHOSE_FIELD);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  threadargs = g_slist_copy(g_slist_nth(args, 1));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, NULL, threadargs);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(url);
}

void twitterapi_s_timeline(GSList *args)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *fields = NULL;
  GSList *threadargs = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;


  track = (g_slist_nth(args, 0))->data;
  locations = (g_slist_nth(args, 1))->data;

  getargs = g_string_new(NULL);
  if(track && strlen(track))
    {
      g_string_append_printf(getargs, "&track=%s", track);
      g_free(track);
    }
  if(locations && strlen(locations))
    {
      g_string_append_printf(getargs, "&locations=%s", locations);
      g_free(locations);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?with=followings&reply=all%s",
			     T_S_USER,
			     getargs->str);
  else
    geturl = g_strdup_printf("%s?with=followings&reply=all",
			     T_S_USER);
  g_string_free(getargs, TRUE);

  if(twitterapi_userfields->fields->len)
    fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_TIMELINE_FIELD);
			     
  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  threadargs = g_slist_copy(g_slist_nth(args, 2));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, NULL, threadargs);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(url);
}

gchar* twitterapi_r_timeline(gchar *count,
			     gchar *since_id,
			     gchar *max_id)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;

  getargs = g_string_new(NULL);
  if(count && strlen(count))
    {
      g_string_append_printf(getargs, "&count=%s", count);
      g_free(count);
    }
  if(since_id && strlen(since_id))
    {
      g_string_append_printf(getargs, "&since_id=%s", since_id);
      g_free(since_id);
    }
  if(max_id && strlen(max_id))
    {
      g_string_append_printf(getargs, "&max_id=%s", max_id);
      g_free(max_id);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_TIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_TIMELINE);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_usersettings(void)
{
  gchar *url = NULL;

  url = oauthapi_sign(T_R_ACCOUNTSETTINGS, NULL);
  return(curlapi_http(url, NULL));
}

gchar* twitterapi_r_trends(gchar *woeid)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  gchar *finalwoeid = NULL;
  gchar *result = NULL;

  if(woeid)
    finalwoeid = g_strdup(woeid);
  else
    {
      if(twitterapi_woeid->fields->len)
	finalwoeid = g_strdup(twitterapi_woeid->fields->str);
      else
	finalwoeid = g_strdup("1");
    }
  geturl = g_strdup_printf(T_R_TRENDS, finalwoeid);
  g_free(finalwoeid);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_woeid(gchar *countryname)
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
      result = curlapi_http(url, NULL);
      /* 
       * if(result && strlen(result) && (result[0] == '[' || result[0] == '{'))
       * 	{
       * 	  JsonParser *parser = jsonapi_parser();
       * 	  JsonNode *node = jsonapi_decode(parser, result);
       * 	  woeid = jsonapi_get_value(node, "..woeid|int");
       * 	}
       * else
       */
	woeid = g_strdup_printf("url=%s, result=%s", url, result);
      g_free(url);
      g_free(result);
    }
  else
    {
      if(twitterapi_woeid->fields->len)
	woeid = g_strdup(twitterapi_woeid->fields->str);
      else
	woeid = NULL;
    }

  return(woeid);
}

gchar* twitterapi_r_search(gchar *q,
			   gchar *count,
			   gchar *geocode,
			   gchar *lang,
			   gchar *locale,
			   gchar *result_type,
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
    {
      g_string_append_printf(getargs, "&q=%s", q);
      g_free(q);
    }
  if(geocode && strlen(geocode))
    {
      g_string_append_printf(getargs, "&geocode=%s", geocode);
      g_free(geocode);
    }
  if(lang && strlen(lang))
    {
      g_string_append_printf(getargs, "&lang=%s", lang);
      g_free(lang);
    }
  if(locale && strlen(locale))
    {
      g_string_append_printf(getargs, "&locale=%s", locale);
      g_free(locale);
    }
  if(result_type && strlen(result_type))
    {
      g_string_append_printf(getargs, "&result_type=%s", result_type);
      g_free(result_type);
    }
  if(count && strlen(count))
    {
      g_string_append_printf(getargs, "&count=%s", count);
      g_free(count);
    }
  if(until && strlen(until))
    {
      g_string_append_printf(getargs, "&until=%s", until);
      g_free(until);
    }
  if(since_id && strlen(since_id))
    {
      g_string_append_printf(getargs, "&since_id=%s", since_id);
      g_free(since_id);
    }
  if(max_id && strlen(max_id))
    {
      g_string_append_printf(getargs, "&max_id=%s", max_id);
      g_free(max_id);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_SEARCH, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_SEARCH);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL);
  g_free(url);
  return(result);
}
