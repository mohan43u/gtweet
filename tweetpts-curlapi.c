#include <tweetpts.h>

static int curlapi_debug(CURL *curlapi,
			 curl_infotype type,
			 char *data,
			 size_t size,
			 void *userdata)
{
  GString *debugbuffer = (GString *) userdata;
  gchar *edata = NULL;

  edata = g_strndup(data, size);
  g_printerr("%s", edata);
  curl_free(edata);
  return(0);
}

static size_t curlapi_http_write_cb(char *ptr,
				    size_t size,
				    size_t nmemb,
				    void *userdata)
{
  gsize length = 0;
  GString *buffer = NULL;
  GSList *args = NULL;
  GSList *threadargs = NULL;
  gboolean (*write_cb)(GSList *args) = NULL;
  gchar *string = NULL;
  gchar *fullstring = NULL;
  
  length = size * nmemb;
  args = (GSList *) userdata;
  buffer = g_slist_nth_data(args, 0);
  write_cb = g_slist_nth_data(args, 1);
  string = g_strndup(ptr, length);
  
  if(g_strcmp0("\r\n", &string[length - 2]) == 0)
    {
      string[length - 2] = '\0';
      g_string_append(buffer, string);
      g_free(string);
      fullstring = g_strdup(buffer->str);
      g_string_set_size(buffer, 0);
      threadargs = g_slist_copy(g_slist_nth(args, 2));
      threadargs = g_slist_append(threadargs, fullstring);

      //Make sure write_cb returns true, else die.
      if(write_cb(threadargs) == FALSE)
	length = 0;
    }
  else
    {
      g_string_append(buffer, string);
      g_free(string);
    }

  return(length);
}

static size_t curlapi_http_write(char *ptr,
				 size_t size,
				 size_t nmemb,
				 void *userdata)
{
  gsize length = size * nmemb;
  GString *buffer = userdata;

  g_string_append_len(buffer, ptr, length);
  g_string_append(buffer, "\0");
  return(length);
}

static struct curl_slist* curlapi_get_oauthheader(gchar **url,
						  gchar **params,
						  gboolean oauth)
{
  if(oauth)
    {
      struct curl_slist *slist = NULL;
      gchar *baseurl = NULL;
      gchar **paramsv = NULL;
      GString *oauthparams = g_string_new(NULL);
      GString *leftparams = g_string_new(NULL);
      guint iter = 0;

      oauthparams = g_string_new("Authorization: OAuth ");
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
	  slist = curl_slist_append(slist, oauthparams->str);
	  slist = curl_slist_append(slist, "Expect: ");
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
      g_string_free(oauthparams, TRUE);
      g_string_free(leftparams, TRUE);
      return(slist);
    }
  else
    return(NULL);
}

void curlapi_http_cb(gchar *inputurl,
		     gchar *inputparams,
		     GSList *args,
		     gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  struct curl_slist *oauthheader = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GSList *threadargs = g_slist_copy(args);
  CURLcode returncode;

  threadargs = g_slist_prepend(threadargs, buffer);
  curl_easy_setopt(curlapi, CURLOPT_USERAGENT, curl_version());
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write_cb);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, threadargs);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  if(params)
    {
      curl_easy_setopt(curlapi, CURLOPT_POST, 1);
      curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
    }
  curl_easy_setopt(curlapi, CURLOPT_FAILONERROR, 1);
  returncode = curl_easy_perform(curlapi);
  if(returncode != CURLE_OK && returncode != CURLE_WRITE_ERROR)
    {
      gchar *httperror = NULL;
      void (*write_cb)(GSList *args) = NULL;

      httperror = curlapi_http(url, params, oauth);
      threadargs = g_slist_remove(threadargs, buffer);
      write_cb = g_slist_nth_data(threadargs, 0);
      threadargs = g_slist_append(threadargs, g_strdup(httperror));
      write_cb(g_slist_nth(threadargs, 1));
      g_free(httperror);
    }

  g_free(url);
  g_free(params);
  g_string_free(buffer, TRUE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_easy_cleanup(curlapi);
}

gchar* curlapi_http(gchar *inputurl,
		    gchar *inputparams,
		    gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  struct curl_slist *oauthheader = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GString *debugbuffer = g_string_new(NULL);
  gchar *string = NULL;
  CURLcode returncode;

  curl_easy_setopt(curlapi, CURLOPT_USERAGENT, curl_version());
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, buffer);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  if(params)
    {
      curl_easy_setopt(curlapi, CURLOPT_POST, 1);
      curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
    }
  returncode = curl_easy_perform(curlapi);
  if(returncode != CURLE_OK && returncode != CURLE_WRITE_ERROR)
    {
      string = g_strdup_printf("CURLcode=%d,"
			       "CURLerror=%s,"
			       "url=%s,"
			       "postparams=%s",
			       returncode,
			       curl_easy_strerror(returncode),
			       url,
			       params);
    }
  else
    {
      if(g_strcmp0("\r\n", &(buffer->str[buffer->len - 2])) == 0)
	g_string_truncate(buffer, buffer->len - 2);
      string = g_strdup(buffer->str);
    }

  g_free(url);
  g_free(params);
  g_string_free(buffer, TRUE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_easy_cleanup(curlapi);
  return(string);
}

gchar* curlapi_http_media(gchar *inputurl,
			  gchar *inputparams,
			  GPtrArray *inputdata,
			  gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  struct curl_slist *oauthheader = NULL;
  struct curl_httppost *firstitem = NULL;
  struct curl_httppost *lastitem = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GString *debugbuffer = g_string_new(NULL);
  gchar *filebuffer = NULL;
  gsize filelength;
  gchar *version = NULL;
  gchar *string = NULL;
  CURLcode returncode;

  curl_easy_setopt(curlapi, CURLOPT_USERAGENT, curl_version());
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, buffer);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  if(params)
    {
      curl_easy_setopt(curlapi, CURLOPT_POST, 1);
      curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
    }
  if(inputdata)
    {
      curlapi_multipart_t *data = NULL;
      guint iter = 0;
      CURLFORMcode ret;
      
      while(iter < inputdata->len)
	{
	  data = (curlapi_multipart_t *) g_ptr_array_index(inputdata, iter);
	  if(data->filepath)
	    {
	      ret = curl_formadd(&firstitem,
				 &lastitem,
				 CURLFORM_COPYNAME,
				 data->name,
				 CURLFORM_FILE,
				 data->filepath,
				 CURLFORM_END);
	    }
	  else
	    {
	      ret = curl_formadd(&firstitem,
				 &lastitem,
				 CURLFORM_COPYNAME,
				 data->name,
				 CURLFORM_COPYCONTENTS,
				 data->contents,
				 CURLFORM_END);
	    }
	  iter++;
	}
    }
  curl_easy_setopt(curlapi, CURLOPT_HTTPPOST, firstitem);
  returncode = curl_easy_perform(curlapi);
  if(returncode != CURLE_OK && returncode != CURLE_WRITE_ERROR)
    {
      string = g_strdup_printf("CURLcode=%d,"
			       "CURLerror=%s,"
			       "url=%s,"
			       "postparams=%s",
			       returncode,
			       curl_easy_strerror(returncode),
			       url,
			       params);
    }
  else
    {
      if(g_strcmp0("\r\n", &(buffer->str[buffer->len - 2])) == 0)
	g_string_truncate(buffer, buffer->len - 2);
      string = g_strdup(buffer->str);
    }

  g_free(url);
  g_free(params);
  g_string_free(buffer, TRUE);
  g_string_free(debugbuffer, TRUE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_formfree(firstitem);
  curl_easy_cleanup(curlapi);
  return(string);
}
