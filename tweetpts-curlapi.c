#include <tweetpts.h>

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
  buffer = (g_slist_nth(args, 0))->data;
  write_cb = (g_slist_nth(args, 1))->data;
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

      oauthparams = g_string_new("Authorization: OAuth realm=\"\", ");
      if(*params)
	{
	  baseurl = g_strdup(*url);
	  paramsv = g_strsplit(*params, "&", 0);
	}
      else
	{
	  gchar **urlv = g_strsplit(*url, "?", 2);
	  baseurl = g_strdup(urlv[0]);
	  paramsv = g_strsplit(urlv[1], "&", 0);
	  g_strfreev(urlv);
	}
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

      if(leftparams->len)
	{
	  g_string_erase(leftparams, 0, 1);
	  if(*params == NULL)
	    *url = g_strdup_printf("%s?%s", baseurl, leftparams->str);
	  if(*params != NULL)
	    {
	      *url = g_strdup(baseurl);
	      *params = g_strdup(leftparams->str);
	    }
	}
      else
	*url = g_strdup(baseurl);

      g_free(baseurl);
      g_string_free(oauthparams, TRUE);
      g_string_free(leftparams, TRUE);
      g_strfreev(paramsv);
      return(slist);
    }
  else
    {
      *url = g_strdup(*url);
      *params = g_strdup(*params);
      return(NULL);
    }
}

void curlapi_http_cb(gchar *url, gchar *params, GSList *args, gboolean oauth)
{
  struct curl_slist *oauthheader = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GSList *threadargs = g_slist_copy(args);
  CURLcode returncode;

  threadargs = g_slist_prepend(threadargs, buffer);
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write_cb);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, threadargs);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  if(params)
    curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
  //curl_easy_setopt(curlapi, CURLOPT_VERBOSE, 1);
  returncode = curl_easy_perform(curlapi);
  if(returncode != CURLE_OK && returncode != CURLE_WRITE_ERROR)
    {
      gchar *httperror = NULL;
      void (*write_cb)(GSList *args) = NULL;

      httperror = curlapi_http(url, params, TRUE);
      g_string_append_printf(buffer,
			     "CURLcode=%d,"
			     "CURLerror=%s,"
			     "url=%s,"
			     "postparams=%s,"
			     "httperror=%s",
			     returncode,
			     curl_easy_strerror(returncode),
			     url,
			     params,
			     httperror);
      if(httperror)
	g_free(httperror);

      threadargs = g_slist_remove(threadargs, buffer);
      write_cb = (g_slist_nth(threadargs, 0))->data;
      threadargs = g_slist_append(threadargs, g_strdup(buffer->str));
      write_cb(g_slist_nth(threadargs, 1));
    }

  g_free(url);
  g_free(params);
  g_slist_free(threadargs);
  g_string_free(buffer, TRUE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_easy_cleanup(curlapi);
}

gchar* curlapi_http(gchar *url, gchar *params, gboolean oauth)
{
  struct curl_slist *oauthheader = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  gchar *string = NULL;
  CURLcode returncode;

  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, buffer);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  if(params)
    curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
  //curl_easy_setopt(curlapi, CURLOPT_VERBOSE, 1);
  if((returncode = curl_easy_perform(curlapi)) != CURLE_OK)
    {
      g_string_append_printf(buffer,
			     "CURLcode=%d,"
			     "CURLerror=%s,"
			     "url=%s,"
			     "postparams=%s",
			     returncode,
			     curl_easy_strerror(returncode),
			     url,
			     params);
    }
  string = g_strdup(buffer->str);
  if(g_strcmp0("\r\n", &string[buffer->len - 2]) == 0)
    string[buffer->len - 2] = '\0';

  g_free(url);
  g_free(params);
  g_string_free(buffer, TRUE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_easy_cleanup(curlapi);
  return(string);
}
