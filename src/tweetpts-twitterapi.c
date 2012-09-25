#include <tweetpts.h>

void twitterapi_s_stat_filter(gchar *track, gchar *follow, gchar *locations,
			      gpointer func, gpointer userdata)
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

  url = oauthapi_sign(T_S_STAT_FILTER, &postparams, "POST");
  curlapi_http_cb(url, postparams, func, userdata, TRUE);
  g_free(postparams);
  g_free(url);
}

void twitterapi_s_stat_sample(gpointer func, gpointer userdata)
{
  gchar *url = NULL;

  url = oauthapi_sign(T_S_STAT_SAMPLE, NULL, "GET");
  curlapi_http_cb(url, NULL, func, userdata, TRUE);
  g_free(url);
}

void twitterapi_s_htimeline(gchar *track, gchar *locations, gpointer func,
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

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  curlapi_http_cb(url, NULL, func, userdata, TRUE);
  g_free(url);
}

gchar* twitterapi_r_htimeline(gchar *count, gchar *since_id, gchar *max_id)
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
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_HTIMELINE, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_HTIMELINE);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_utimeline(gchar *userid, gchar *screenname, gchar *since_id,
			      gchar *count, gchar *max_id)
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

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_usersettings(void)
{
  gchar *url = NULL;
  gchar *result = NULL;

  url = oauthapi_sign(T_R_ACCOUNTSETTINGS, NULL, "GET");
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
	  g_object_unref(parser);
      	}
      else
	woeid = NULL;

      g_free(url);
      g_free(result);
    }
  else
    woeid = NULL;

  return(woeid);
}

gchar* twitterapi_r_trends(gchar *woeid)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  gchar *result = NULL;

  geturl = g_strdup_printf(T_R_TRENDS, woeid);
  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_tweetsearch(gchar *q, gchar *geocode, gchar *lang,
				gchar *locale, gchar *result_type, gchar *count,
				gchar *until, gchar *since_id, gchar *max_id)
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

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_lookup(gchar *screenname, gchar *user_id)
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

  url = oauthapi_sign(T_R_LOOKUP, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  g_free(postparams);
  return(result);
}

gchar* twitterapi_r_usersearch(gchar *q, gchar *page, gchar *count)
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

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

static gchar* twitterapi_strsplit(gchar **str, gchar *delimit, guint length)
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

gchar* twitterapi_r_following(gchar *userid, gchar *screenname, gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;
  GString *buffer = NULL;
  gchar *finalresult = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    g_string_append_printf(getargs, "&user_id=%s", userid);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(cursor && strlen(cursor))
    g_string_append_printf(getargs, "&cursor=%s", cursor);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_FOLLOWING, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWING);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL, "GET");
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

gchar* twitterapi_r_followers(gchar *userid, gchar *screenname, gchar *cursor)
{
  gchar *url = NULL;
  gchar *geturl = NULL;
  GString *getargs = NULL;
  gchar *result = NULL;
  GString *buffer = NULL;
  gchar *finalresult = NULL;

  getargs = g_string_new(NULL);
  if(userid && strlen(userid))
    g_string_append_printf(getargs, "&user_id=%s", userid);
  if(screenname && strlen(screenname))
    g_string_append_printf(getargs, "&screen_name=%s", screenname);
  if(cursor && strlen(cursor))
    g_string_append_printf(getargs, "&cursor=%s", cursor);
  if(getargs->len)
    geturl = g_strdup_printf("%s?%s", T_R_FOLLOWERS, &(getargs->str[1]));
  else
    geturl = g_strdup(T_R_FOLLOWERS);
  g_string_free(getargs, TRUE);

  url = oauthapi_sign(geturl, NULL, "GET");
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

gchar* twitterapi_r_updatemedia(gchar *status, gchar *filepath)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
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
    }
  if(status && strlen(status))
    {
      data = g_new0(curlapi_multipart_t, 1);
      data->name = g_strdup("status");
      data->contents = g_strdup(status);
      g_ptr_array_add(inputdata, data);
    }

  posturl = g_strdup(T_R_UPDATEMEDIA);
  postparams = g_strdup("");
  url = oauthapi_sign(posturl, &postparams, "POST");
  g_free(posturl);
  result = curlapi_http_media(url, postparams, inputdata, TRUE);
  g_free(url);
  g_free(postparams);

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

gchar* twitterapi_r_update(gchar *status, gchar *replypostid)
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

  url = oauthapi_sign(T_R_UPDATE, &postparams, "POST");
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
    posturl = g_strdup_printf(T_R_RETWEET, postid);
  else
    return(NULL);

  postparams = g_strdup("");
  url = oauthapi_sign(posturl, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(postparams);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_destroy(gchar *postid)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  gchar *result = NULL;

  if(postid && strlen(postid))
    posturl = g_strdup_printf(T_R_DESTROY, postid);
  else
    return(NULL);

  postparams = g_strdup("");
  url = oauthapi_sign(posturl, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_follow(gchar *screenname, gchar *userid)
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

  url = oauthapi_sign(T_R_FOLLOW, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_unfollow(gchar *screenname, gchar *userid)
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

  url = oauthapi_sign(T_R_UNFOLLOW, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_blocklist(gchar *cursor)
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

  url = oauthapi_sign(geturl, NULL, "GET");
  g_free(geturl);
  result = curlapi_http(url, NULL, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_block(gchar *screenname, gchar *userid)
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

  url = oauthapi_sign(T_R_BLOCK, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_unblock(gchar *screenname, gchar *userid)
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

  url = oauthapi_sign(T_R_UNBLOCK, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_profile(gchar *name, gchar *purl, gchar *location,
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

  url = oauthapi_sign(T_R_PROFILE, &postparams, "POST");
  result = curlapi_http(url, postparams, TRUE);
  g_free(url);
  return(result);
}

gchar* twitterapi_r_pbackground(gchar *filepath, gchar *tile, gchar *use)
{
  gchar *url = NULL;
  gchar *posturl = NULL;
  gchar *postparams = NULL;
  GString *postargs = NULL;
  GPtrArray *inputdata = NULL;
  curlapi_multipart_t *data = NULL;
  gchar *result = NULL;
  gchar iter = 0;

  inputdata = g_ptr_array_new();
  if(filepath && strlen(filepath))
    {
      data = g_new0(curlapi_multipart_t, 1);
      data->name = g_strdup("image");
      data->filepath = glibapi_expandfilename(filepath);
      g_ptr_array_add(inputdata, data);
    }

  posturl = g_strdup(T_R_PBACKGROUND);
  postparams = g_strdup("");
  url = oauthapi_sign(posturl, &postparams, "POST");
  g_free(posturl);
  result = curlapi_http_media(url, postparams, inputdata, TRUE);
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
      url = oauthapi_sign(posturl, &postparams, "POST");
      g_free(posturl);
      result = curlapi_http(url, postparams, TRUE);
    }
  g_free(url);
  g_free(postparams);

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

void twitterapi_init(void)
{
  glibapi_init();
  oauthapi_init();
  jsonapi_init();
}

void twitterapi_free(void)
{
  jsonapi_free();
  oauthapi_free();
  glibapi_free();
}
