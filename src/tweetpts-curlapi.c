/* 
 * Copyright (c) <2012>, Mohan R <mohan43u@gmail.com>
 * All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 * 
 *   1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 *   of the authors and should not be interpreted as representing official policies,
 *   either expressed or implied, of the FreeBSD Project.
 */

#include <tweetpts.h>

static int curlapi_debug(CURL *curlapi, curl_infotype type, char *data,
			 size_t size, void *userdata)
{
  GString *debugbuffer = (GString *) userdata;
  gchar *edata = NULL;

  edata = g_strndup(data, size);
  g_printerr("%s", edata);
  curl_free(edata);
  return(0);
}

static size_t curlapi_http_write_cb(char *ptr, size_t size, size_t nmemb,
				    void *userdata)
{
  GPtrArray *cbargs = NULL;
  gsize length = 0;
  GString *buffer = NULL;
  gboolean (*write_cb)(gchar *string, gpointer userdata) = NULL;
  gpointer writecb_userdata = NULL;
  gchar *string = NULL;
  gchar *fullstring = NULL;
  
  length = size * nmemb;
  cbargs = userdata;
  buffer = g_ptr_array_index(cbargs, 0);
  write_cb = g_ptr_array_index(cbargs, 1);
  writecb_userdata = g_ptr_array_index(cbargs, 2);
  string = g_strndup(ptr, length);
  
  if(g_strcmp0("\r\n", &string[length - 2]) == 0)
    {
      string[length - 2] = '\0';
      g_string_append(buffer, string);
      g_free(string);
      fullstring = g_strdup(buffer->str);
      g_string_set_size(buffer, 0);
      //Make sure write_cb returns true, else die.
      if(write_cb(fullstring, writecb_userdata) == FALSE)
	length = 0;
      g_free(fullstring);
    }
  else
    {
      g_string_append(buffer, string);
      g_free(string);
    }

  return(length);
}

static size_t curlapi_http_write(char *ptr, size_t size, size_t nmemb,
				 void *userdata)
{
  gsize length = size * nmemb;
  GString *buffer = userdata;

  g_string_append_len(buffer, ptr, length);
  g_string_append(buffer, "\0");
  return(length);
}

static struct curl_slist* curlapi_get_oauthheader(gchar **url, gchar **params,
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

void curlapi_http_cb(gchar *inputurl, gchar *inputparams, gpointer func,
		     gpointer userdata, gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  struct curl_slist *oauthheader = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GPtrArray *cbargs = g_ptr_array_new();
  CURLcode returncode;

  g_ptr_array_add(cbargs, buffer);
  g_ptr_array_add(cbargs, func);
  g_ptr_array_add(cbargs, userdata);
  curl_easy_setopt(curlapi, CURLOPT_USERAGENT, curl_version());
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write_cb);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, cbargs);
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
      gboolean (*write_cb)(gchar *string, gpointer userdata) = NULL;

      httperror = curlapi_http(url, params, oauth);
      write_cb = func;
      write_cb(httperror, userdata);
      g_free(httperror);
    }

  g_free(url);
  g_free(params);
  g_string_free(buffer, TRUE);
  g_ptr_array_free(cbargs, FALSE);
  if(oauthheader)
    curl_slist_free_all(oauthheader);
  curl_easy_cleanup(curlapi);
}

gchar* curlapi_http(gchar *inputurl, gchar *inputparams, gboolean oauth)
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

gchar* curlapi_http_media(gchar *inputurl, gchar *inputparams,
			  GPtrArray *inputdata, gboolean oauth)
{
  gchar *url = g_strdup(inputurl);
  gchar *params = g_strdup(inputparams);
  struct curl_slist *oauthheader = NULL;
  struct curl_httppost *firstitem = NULL;
  struct curl_httppost *lastitem = NULL;
  CURL *curlapi = curl_easy_init();
  GString *buffer = g_string_new(NULL);
  GString *debugbuffer = g_string_new(NULL);
  gchar *version = NULL;
  gchar *string = NULL;
  CURLcode returncode;

  curl_easy_setopt(curlapi, CURLOPT_USERAGENT, curl_version());
  curl_easy_setopt(curlapi, CURLOPT_WRITEFUNCTION, curlapi_http_write);
  curl_easy_setopt(curlapi, CURLOPT_WRITEDATA, buffer);
  oauthheader = curlapi_get_oauthheader(&url, &params, oauth);
  curl_easy_setopt(curlapi, CURLOPT_URL, url);
  curl_easy_setopt(curlapi, CURLOPT_HTTPHEADER, oauthheader);
  curl_easy_setopt(curlapi, CURLOPT_FAILONERROR, 1);
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
