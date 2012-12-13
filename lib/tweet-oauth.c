/*
 * gtweet - GLib/GObject wrapper for twitter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <tweet.h>

gchar* tweet_oauth_expandfilename(gchar *filename)
{
  wordexp_t result_t;
  gchar *result = NULL;
  if(wordexp(filename, &result_t, 0) == 0)
    result = g_strjoinv(" ", result_t.we_wordv);
  else
    result = g_strdup(filename);
  wordfree(&result_t);
  return result;
}

gchar* tweet_oauth_sign(gchar *consumer_key,
			gchar *consumer_secret,
			gchar *access_key,
			gchar *access_secret,
			gchar *url,
			gchar **params,
			gchar *method)
{
  gchar *requesturl = NULL;
  gchar *resulturl = NULL;

  if(params)
    requesturl = g_strdup_printf("%s?%s", url, *params);
  else
    requesturl = g_strdup(url);

  resulturl = oauth_sign_url2(requesturl,
			      params,
			      OA_HMAC,
			      method,
			      consumer_key,
			      consumer_secret,
			      access_key,
			      access_secret);

  g_free(requesturl);
  return resulturl;
}

void tweet_oauth_request_token(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar **request_key,
			       gchar **request_secret)
{
  gchar *request = NULL;
  gchar *response = NULL;
  gchar **token = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar *postarg = NULL;

  request = g_strdup_printf("%s?%s", REQ_TOKEN_URL, "oauth_callback=oob");
  oauth_sign_url2(request,
		  &postarg,
		  OA_HMAC,
		  NULL,
		  consumer_key,
		  consumer_secret,
		  NULL,
		  NULL);

  response = tweet_soup_sync(REQ_TOKEN_URL, postarg, FALSE);
  argc = oauth_split_post_paramters(response, &argv, 0);

  while(argc)
    {
      token = g_strsplit(argv[argc - 1], "=", 0);
      if(g_strcmp0(token[0], "oauth_token") == 0)
	*request_key = g_strdup(token[1]);
      if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	*request_secret = g_strdup(token[1]);
      argc--;
    }

  g_free(request);
  g_free(postarg);
  g_free(response);
  g_strfreev(token);
  oauth_free_array(&argc, &argv);
}

gchar* tweet_oauth_gen_authurl(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *request_key,
			       gchar *request_secret)
{
  gchar *authurl = NULL;
  gchar *request = NULL;

  request = oauth_sign_url2(AUTH_URL,
			    NULL,
			    OA_HMAC,
			    NULL,
			    consumer_key,
			    consumer_secret,
			    request_key,
			    request_secret);
  authurl = g_strdup_printf("%s&"
			    "oauth_token=%s&"
			    "oauth_token_secret=%s",
			    request,
			    request_key,
			    request_secret);

  g_free(request);
  return authurl;
}

void tweet_oauth_access_token(gchar *pin,
			      gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *request_key,
			      gchar *request_secret,
			      gchar **access_key,
			      gchar **access_secret)
{
  gchar *request = NULL;
  gchar *response = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar *postarg = NULL;
  gchar **token = NULL;

  request = g_strdup_printf("%s?oauth_verifier=%s", ACCESS_TOKEN_URL, pin);
  oauth_sign_url2(request,
		  &postarg,
		  OA_HMAC,
		  NULL,
		  consumer_key,
		  consumer_secret,
		  request_key,
		  request_secret);

  response = tweet_soup_sync(ACCESS_TOKEN_URL, postarg, TRUE);
  argc = oauth_split_post_paramters(response, &argv, 0);

  while(argc)
    {
      token = g_strsplit(argv[argc - 1], "=", 0);
      if(g_strcmp0(token[0], "oauth_token") == 0)
	*access_key = g_strdup(token[1]);
      if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	*access_secret = g_strdup(token[1]);
      argc--;
    }

  if(access_key == NULL || access_secret == NULL)
    {
      g_print("things went wrong!! not able to get access_{key,secret}!!\n");
      exit(EXIT_FAILURE);
    }

  g_free(request);
  g_free(postarg);
  g_free(response);
  g_strfreev(token);
  oauth_free_array(&argc, &argv);
}

void tweet_oauth_to_file(gchar *appname,
			 gchar *consumer_key,
			 gchar *consumer_secret,
			 gchar *access_key,
			 gchar *access_secret)
{
  gchar *content = NULL;
  gchar *oauthfile = NULL;

  content = g_strdup_printf("appname=%s&"
			    "consumer_key=%s&consumer_secret=%s&"
			    "access_key=%s&access_secret=%s",
			    appname,
			    consumer_key,
			    consumer_secret,
			    access_key,
			    access_secret);

  oauthfile = tweet_oauth_expandfilename(OAUTHFILE);
  g_file_set_contents(oauthfile,
		      content,
		      strlen(content),
		      NULL);

  g_free(content);
  g_free(oauthfile);
}

gboolean tweet_oauth_from_file(gchar **appname,
			       gchar **consumer_key,
			       gchar **consumer_secret,
			       gchar **access_key,
			       gchar **access_secret)
{
  gchar *response = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar **pair = NULL;
  gboolean result = FALSE;
  gchar *oauthfile = NULL;

  oauthfile = tweet_oauth_expandfilename(OAUTHFILE);

  if(g_file_get_contents(oauthfile, &response, NULL, NULL))
    {
      argc = oauth_split_post_paramters(response, &argv, 0);
      while(argc)
	{
	  pair = g_strsplit(argv[argc - 1], "=", 0);
	  if(g_strcmp0(pair[0], "appname") == 0)
	    *appname = g_strdup(pair[1]);
	  if(g_strcmp0(pair[0], "consumer_key") == 0)
	    *consumer_key = g_strdup(pair[1]);
	  if(g_strcmp0(pair[0], "consumer_secret") == 0)
	    *consumer_secret = g_strdup(pair[1]);
	  if(g_strcmp0(pair[0], "access_key") == 0)
	    *access_key = g_strdup(pair[1]);
	  if(g_strcmp0(pair[0], "access_secret") == 0)
	    *access_secret = g_strdup(pair[1]);
	  argc--;
	  g_strfreev(pair);
	}
      if(*appname &&
	 *consumer_key &&
	 *consumer_secret &&
	 *access_key &&
	 *access_secret)
	result = TRUE;

      oauth_free_array(&argc, &argv);
    }

  g_free(response);
  g_free(oauthfile);
  return result;
}
