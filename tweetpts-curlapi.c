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

void curlapi_http_cb(gchar *url, gchar *params, GSList *args)
{
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GSList *threadargs = g_slist_copy(args);
  CURLcode returncode;

  threadargs = g_slist_prepend(threadargs, buffer);
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write_cb);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, threadargs);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  if(params)
    curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);

  returncode = curl_easy_perform(curlapi);
  if(returncode != CURLE_OK && returncode != CURLE_WRITE_ERROR)
    {
      gchar *httperror = NULL;
      void (*write_cb)(GSList *args) = NULL;

      httperror = curlapi_http(url, params);
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
      
  g_slist_free(threadargs);
  g_string_free(buffer, TRUE);
  curl_easy_cleanup(curlapi);
}

gchar* curlapi_http(gchar *url, gchar *params)
{
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  gchar *string = NULL;
  CURLcode returncode;

  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, buffer);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  if(params)
    curl_easy_setopt(curlapi, CURLOPT_POSTFIELDS, params);
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
  g_string_free(buffer, TRUE);
  curl_easy_cleanup(curlapi);
  return(string);
}
