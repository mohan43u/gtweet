#include <tweetpts.h>

void twitterapi_init(void)
{
  twitterapi_userfields = g_new0(twitterapi_field_t, 1);
  twitterapi_userfields->fields = g_string_new(NULL);
  g_mutex_init(&(twitterapi_userfields->mutex));

  twitterapi_woeid = g_new0(twitterapi_field_t, 1);
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
      string = g_strdup(twitterapi_userfields->fields->str);
      g_string_set_size(twitterapi_userfields->fields, 0);
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

  track = g_slist_nth_data(args, 0);
  follow = g_slist_nth_data(args, 1);
  locations = g_slist_nth_data(args, 2);

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
    fields = g_strdup(twitterapi_get_userfields());
  else
    fields = g_strdup(T_FILTER_FIELD);

  url = oauthapi_sign(T_S_STAT_FILTER, &searchpattern);
  threadargs = g_slist_copy(g_slist_nth(args, 3));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, searchpattern, threadargs, TRUE);
  g_slist_free(threadargs);
  g_free(searchpattern);
  g_free(fields);
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
  curlapi_http_cb(url, NULL, threadargs, TRUE);
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
  
  count = g_slist_nth_data(args, 0);

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
  curlapi_http_cb(url, NULL, threadargs, TRUE);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(url);
}

void twitterapi_s_htimeline(GSList *args)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *fields = NULL;
  GSList *threadargs = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;


  track = g_slist_nth_data(args, 0);
  locations = g_slist_nth_data(args, 1);

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
    fields = g_strdup(T_HTIMELINE_FIELD);
			     
  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  threadargs = g_slist_copy(g_slist_nth(args, 2));
  threadargs = g_slist_append(threadargs, fields);
  curlapi_http_cb(url, NULL, threadargs, TRUE);
  g_slist_free(threadargs);
  g_free(fields);
  g_free(url);
}

gchar* twitterapi_r_htimeline(gchar *count,
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
    geturl = g_strdup_printf("%s?%s", T_R_HTIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_HTIMELINE);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_utimeline(gchar *userid,
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
    {
      g_string_append_printf(getargs, "&user_id=%s", userid);
      g_free(userid);
    }
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(getargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(since_id && strlen(since_id))
    {
      g_string_append_printf(getargs, "&since_id=%s", since_id);
      g_free(since_id);
    }
  if(count && strlen(count))
    {
      g_string_append_printf(getargs, "&count=%s", count);
      g_free(count);
    }
  if(max_id && strlen(max_id))
    {
      g_string_append_printf(getargs, "&max_id=%s", max_id);
      g_free(max_id);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_UTIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_UTIMELINE);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_usersettings(void)
{
  gchar *url = NULL;
  gchar *result = NULL;

  url = oauthapi_sign(T_R_ACCOUNTSETTINGS, NULL);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
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
  result = curlapi_http(url, NULL, TRUE);
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
      result = curlapi_http(url, NULL, FALSE);
      if(result && strlen(result) && result[0] == '{')
      	{
      	  JsonParser *parser = jsonapi_parser();
      	  JsonNode *node = jsonapi_decode(parser, result);
      	  woeid = jsonapi_get_value(node, "$..woeid|int");
      	}
      else
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

gchar* twitterapi_r_tweetsearch(gchar *q,
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
    geturl = g_strdup_printf("%s?%s", T_R_TWEETSEARCH, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_TWEETSEARCH);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_lookup(gchar *screenname,
			   gchar *user_id)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(postargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(user_id && strlen(user_id))
    {
      g_string_append_printf(postargs, "&user_id=%s", user_id);
      g_free(user_id);
    }
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = oauthapi_sign(T_R_LOOKUP, &postparams);
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  g_free(postparams);
  return(result);
}

gchar* twitterapi_r_usersearch(gchar *q,
			       gchar *page,
			       gchar *count)
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
  if(page && strlen(page))
    {
      g_string_append_printf(getargs, "&page=%s", page);
      g_free(page);
    }
  if(count && strlen(count))
    {
      g_string_append_printf(getargs, "&count=%s", count);
      g_free(count);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_USERSEARCH, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_USERSEARCH);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

static gchar* twitterapi_strsplit(gchar **str,
				  gchar *delimit,
				  guint length)
{
  gchar **strv = NULL;
  guint strc = 0;
  gchar *result = NULL;

  strv = g_strsplit(*str, delimit, length);
  g_free(*str);
  *str = NULL;

  while(strv[strc]) strc++;
  if(strc == length)
    {
      *str = g_strdup(strv[length - 1]);
      g_free(strv[length - 1]);
      strv[length - 1] = NULL;
    }
  result = g_strjoinv(",", strv);
  g_strfreev(strv);
  return(result);
}

gchar* twitterapi_r_following(gchar *userid,
			      gchar *screenname,
			      gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;
  GString *buffer = NULL;
  gchar *finalresult = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    {
      g_string_append_printf(getargs, "&user_id=%s", userid);
      g_free(userid);
    }
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(getargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(cursor && strlen(cursor))
    {
      g_string_append_printf(getargs, "&cursor=%s", cursor);
      g_free(cursor);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_FOLLOWING, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWING);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);

  buffer = g_string_new(NULL);
  if(result && strlen(result) && result[0] == '{')
    {
      JsonParser *parser = jsonapi_parser();
      JsonNode *root = jsonapi_decode(parser, result);
      JsonNode *child = jsonapi_get_object(root, "$.ids");
      gchar *ids = jsonapi_get_array(child, "$.*|int");
      gchar *limitids = NULL;
      gchar *limitresult = NULL;

      while(ids)
	{
	  limitids = twitterapi_strsplit(&ids, ",", 101);
	  limitresult = twitterapi_r_lookup(NULL, limitids);
	  g_string_append_printf(buffer, "%s\n", limitresult);
	  g_free(limitresult);
	}
	  
      json_node_free(child);
      g_object_unref(parser);
    }
  else
    buffer = g_string_append(buffer, result);

  g_free(url);
  g_free(result);
  if(buffer->len)
    {
      g_string_truncate(buffer, buffer->len - 1);
      finalresult = g_strdup(buffer->str);
    }
  g_string_free(buffer, TRUE);
  return(finalresult);
}

gchar* twitterapi_r_followers(gchar *userid,
			      gchar *screenname,
			      gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;
  GString *buffer = NULL;
  gchar *finalresult = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    {
      g_string_append_printf(getargs, "&user_id=%s", userid);
      g_free(userid);
    }
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(getargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(cursor && strlen(cursor))
    {
      g_string_append_printf(getargs, "&cursor=%s", cursor);
      g_free(cursor);
    }
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_FOLLOWERS, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWERS);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);

  buffer = g_string_new(NULL);
  if(result && strlen(result) && result[0] == '{')
    {
      JsonParser *parser = jsonapi_parser();
      JsonNode *root = jsonapi_decode(parser, result);
      JsonNode *child = jsonapi_get_object(root, "$.ids");
      gchar *ids = jsonapi_get_array(child, "$.*|int");
      gchar *limitids = NULL;
      gchar *limitresult = NULL;

      while(ids)
	{
	  limitids = twitterapi_strsplit(&ids, ",", 101);
	  limitresult = twitterapi_r_lookup(NULL, limitids);
	  g_string_append_printf(buffer, "%s\n", limitresult);
	  g_free(limitresult);
	}
	  
      json_node_free(child);
      g_object_unref(parser);
    }
  else
    buffer = g_string_append(buffer, result);

  g_free(url);
  g_free(result);
  if(buffer->len)
    {
      g_string_truncate(buffer, buffer->len - 1);
      finalresult = g_strdup(buffer->str);
    }
  g_string_free(buffer, TRUE);
  return(finalresult);
}

gchar* twitterapi_r_updatemedia(gchar *status,
				gchar *filepath)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GPtrArray *inputdata = NULL;
  curlapi_multipart_t *data = NULL;
  gchar *result = NULL;
  gchar iter = 0;

  inputdata = g_ptr_array_new();
  if(filepath && strlen(filepath))
    {
      data = g_new0(curlapi_multipart_t, 1);
      data->name = g_strdup("media[]");
      data->filepath = glibapi_expandfilename(filepath);
      g_ptr_array_add(inputdata, data);
      g_free(filepath);
    }
  if(status && strlen(status))
    {
      data = g_new0(curlapi_multipart_t, 1);
      data->name = g_strdup("status");
      data->contents = g_strdup(status);
      g_ptr_array_add(inputdata, data);
      g_free(status);
    }
  geturl = g_strdup(T_R_UPDATEMEDIA);

  url = oauthapi_sign(geturl, NULL);
  g_free(geturl);
  result = curlapi_http_media(url, NULL, inputdata, TRUE);
  g_free(url);

  iter = 0;
  while(iter < inputdata->len)
    {
      data = (curlapi_multipart_t *) g_ptr_array_index(inputdata, iter);
      g_free(data->name);
      g_free(data->contenttype);
      g_free(data->contents);
      g_free(data->filepath);
      g_free(data);
      iter++;
    }
  g_ptr_array_free(inputdata, FALSE);
  return(result);
}

gchar* twitterapi_r_update(gchar *status)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(status && strlen(status))
    {
      g_string_append_printf(postargs, "&status=%s", status);
      g_free(status);
    }
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = oauthapi_sign(T_R_UPDATE, &postparams);
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_retweet(gchar *postid)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  gchar *result = NULL;

  if(postid && strlen(postid))
    {
      posturl = g_strdup_printf(T_R_RETWEET, postid);
      g_free(postid);
    }
  else
    return(NULL);

  postparams = g_strdup("");
  url = oauthapi_sign(posturl, &postparams);
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_follow(gchar *screenname,
			   gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(postargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(userid && strlen(userid))
    {
      g_string_append_printf(postargs, "&user_id=%s", userid);
      g_free(userid);
    }
  if(postargs->len)
    postparams = g_strdup_printf("follow=true%s", postargs->str);
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = oauthapi_sign(T_R_FOLLOW, &postparams);
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_unfollow(gchar *screenname,
			     gchar *userid)
{
  gchar *url = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  gchar *result = NULL;

  postargs = g_string_new(NULL);
  if(screenname && strlen(screenname))
    {
      g_string_append_printf(postargs, "&screen_name=%s", screenname);
      g_free(screenname);
    }
  if(userid && strlen(userid))
    {
      g_string_append_printf(postargs, "&user_id=%s", userid);
      g_free(userid);
    }
  if(postargs->len)
    postparams = g_strdup(&(postargs->str[1]));
  else
    postparams = g_strdup("");
  g_string_free(postargs, TRUE);

  url = oauthapi_sign(T_R_UNFOLLOW, &postparams);
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}
