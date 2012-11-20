#include <tweet.h>

static gchar* tweet_soup_get_oauthheader(gchar **url,
					 gchar **params,
					 gboolean oauth)
{
  if(oauth)
    {
      gchar *baseurl = NULL;
      gchar **paramsv = NULL;
      GString *oauthparams = g_string_new(NULL);
      GString *leftparams = g_string_new(NULL);
      gchar *result = NULL;
      guint iter = 0;

      oauthparams = g_string_new("OAuth ");
      if(*params)
	{
	  baseurl = g_strdup(*url);
	  paramsv = g_strsplit(*params, "&", 0);
	}
      else
	{
	  gchar **urlv = NULL;

	  urlv = g_strsplit(*url, "?", 2);
	  baseurl = g_strdup(urlv[0]);
	  if(urlv[1] && strlen(urlv[1]))
	    paramsv = g_strsplit(urlv[1], "&", 0);
	  g_strfreev(urlv);
	}

      if(paramsv)
	{
	  while(paramsv[iter])
	    {
	      if(g_str_has_prefix(paramsv[iter], "oauth_"))
		{
		  gchar **fieldv = g_strsplit(paramsv[iter], "=", 2);
		  g_string_append_printf(oauthparams,
					 "%s=\"%s\", ",
					 fieldv[0],
					 fieldv[1]);
		  g_strfreev(fieldv);
		}
	      else
		g_string_append_printf(leftparams, "&%s", paramsv[iter]);

	      iter++;
	    }
	  g_string_truncate(oauthparams, oauthparams->len - 2);
	  if(leftparams->len)
	    g_string_erase(leftparams, 0, 1);

	  if(*params == NULL)
	    {
	      g_free(*url);
	      if(leftparams->len)
		*url = g_strdup_printf("%s?%s", baseurl, leftparams->str);
	      else
		*url = g_strdup(baseurl);
	    }
	  else
	    {
	      g_free(*url);
	      *url = g_strdup(baseurl);
	      if(leftparams->len)
		{
		  g_free(*params);
		  *params = g_strdup(leftparams->str);
		}
	      else if(*params != NULL)
		{
		  g_free(*params);
		  *params = g_strdup("");
		}
	      else
		{
		  g_free(*params);
		  *params = NULL;
		}
	    }
	  g_strfreev(paramsv);
	}

      g_free(baseurl);
      g_string_free(leftparams, TRUE);
      result = g_strdup(oauthparams->str);
      g_string_free(oauthparams, TRUE);
      return result;
    }
  else
    return NULL;
}

static void tweet_soup_async_got_chunk_cb(SoupMessage *msg,
					  SoupBuffer *chunk,
					  gpointer userdata)
{
  GPtrArray *cbargs = NULL;
  GString *buffer = NULL;
  gboolean (*write_cb)(gchar *string, gsize length, gpointer userdata) = NULL;
  gpointer writecb_userdata = NULL;
  gchar *string = NULL;
  gsize length = 0;
  gchar *fullstring = NULL;
  
  cbargs = (GPtrArray *) userdata;
  buffer = g_ptr_array_index(cbargs, 0);
  write_cb = g_ptr_array_index(cbargs, 1);
  writecb_userdata = g_ptr_array_index(cbargs, 2);
  length = chunk->length;
  string = g_strndup(chunk->data, length);
  
  if(g_strcmp0("\r\n", &(string[length - 2])) == 0)
    {
      string[length - 2] = '\0';
      g_string_append(buffer, string);
      fullstring = g_strdup(buffer->str);
      length = buffer->len;
      g_string_set_size(buffer, 0);
      //Make sure write_cb returns true, else die.
      if(write_cb(fullstring, length, writecb_userdata) == FALSE)
	soup_session_cancel_message(tweet_soup_session, msg, SOUP_STATUS_CANCELLED);
      g_free(fullstring);
    }
  else
      g_string_append(buffer, string);

  g_free(string);
}

void tweet_soup_init(void)
{
  tweet_soup_session = soup_session_sync_new();
  tweet_soup_logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
  soup_logger_attach(tweet_soup_logger,
  		     tweet_soup_session);
}

void tweet_soup_free(void)
{
  soup_logger_detach(tweet_soup_logger,
  		     tweet_soup_session);
  g_object_unref(tweet_soup_logger);
  g_object_unref(tweet_soup_session);
}

void tweet_soup_async(gchar *inputurl,
		      gchar *inputparams,
		      gpointer func,
		      gpointer user_data,
		      gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  GString *buffer = g_string_new(NULL);
  GPtrArray *cbargs = g_ptr_array_new();
  SoupMessage *msg = NULL;
  gchar *value = NULL;
  guint http_code = 0;

  g_ptr_array_add(cbargs, buffer);
  g_ptr_array_add(cbargs, func);
  g_ptr_array_add(cbargs, user_data);

  value = tweet_soup_get_oauthheader(&url, &params, oauth);
  if(inputparams)
    {
      msg = soup_message_new("POST", url);
      soup_message_set_request(msg,
			       "application/x-www-form-urlencoded",
			       SOUP_MEMORY_COPY,
			       params,
			       strlen(params));
    }
  else
    msg = soup_message_new("GET", url);
  soup_message_headers_append(msg->request_headers,
			      "Authorization", value);
  g_object_connect(G_OBJECT(msg),
		   "signal::got-chunk", tweet_soup_async_got_chunk_cb, cbargs,
		   NULL);
  http_code = soup_session_send_message(tweet_soup_session,
					msg);
  if(http_code >= 400)
    g_printerr("error: http_code returned as %d\n", http_code);

  g_free(url);
  g_free(params);
  g_free(value);
  g_string_free(buffer, TRUE);
  g_ptr_array_free(cbargs, FALSE);
  g_object_unref(msg);
}

gchar* tweet_soup_sync(gchar *inputurl,
		       gchar *inputparams,
		       gboolean oauth)
{
  GString *buffer = tweet_soup_gstring_sync(inputurl, inputparams, oauth);
  gchar *string = g_strdup(buffer->str);
  g_string_free(buffer, TRUE);
  return string;
}

GString* tweet_soup_gstring_sync(gchar *inputurl,
				 gchar *inputparams,
				 gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  GString *buffer = g_string_new(NULL);
  SoupMessage *msg = NULL;
  gchar *value = NULL;
  guint http_code = 0;

  value = tweet_soup_get_oauthheader(&url, &params, oauth);
  if(inputparams)
    {
      msg = soup_message_new("POST", url);
      soup_message_set_request(msg,
			       "application/x-www-form-urlencoded",
			       SOUP_MEMORY_COPY,
			       params,
			       strlen(params));
    }
  else
    msg = soup_message_new("GET", url);
  soup_message_headers_append(msg->request_headers,
			      "Authorization", value);
  http_code = soup_session_send_message(tweet_soup_session,
					msg);
  if(http_code >= 400)
      g_printerr("error: http_code returned as %d\n", http_code);
  g_string_append_len(buffer,
		      msg->response_body->data,
		      msg->response_body->length);

  g_free(url);
  g_free(params);
  g_free(value);
  g_object_unref(msg);

  return buffer;
}

gchar* tweet_soup_sync_media(gchar *inputurl,
			     gchar *inputparams,
			     gboolean oauth,
			     gchar *status,
			     gchar *filename)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  GString *buffer = g_string_new(NULL);
  gchar *filecontent = NULL;
  SoupBuffer *filebuffer = NULL;
  gsize filelength;
  SoupMultipart *multi = NULL;
  SoupMessage *msg = NULL;
  gchar *value = NULL;
  guint http_code = 0;
  gchar *string = NULL;

  value = tweet_soup_get_oauthheader(&url, &params, oauth);
  if(inputparams)
    msg = soup_message_new("POST", url);
  else
    msg = soup_message_new("GET", url);

  g_file_get_contents(filename, &filecontent, &filelength, NULL);
  filebuffer = soup_buffer_new(SOUP_MEMORY_TAKE, filecontent, filelength);
  multi = soup_multipart_new("multipart/form-data");

  if(filename)
    soup_multipart_append_form_file(multi,
				    "media[]",
				    filename,
				    NULL,
				    filebuffer);
  if(status)
    soup_multipart_append_form_string(multi,
				      "status",
				      status);

  soup_multipart_to_message(multi,
			    msg->request_headers,
			    msg->request_body);
  soup_message_headers_append(msg->request_headers,
			      "Authorization", value);
  http_code = soup_session_send_message(tweet_soup_session,
					msg);
  if(http_code >= 400)
      g_printerr("error: http_code returned as %d\n", http_code);
  g_string_append_len(buffer,
		      msg->response_body->data,
		      msg->response_body->length);
  string = g_strdup(buffer->str);

  g_free(url);
  g_free(params);
  g_free(value);
  g_object_unref(msg);
  g_string_free(buffer, TRUE);

  return string;
}
