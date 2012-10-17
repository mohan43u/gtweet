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
  return(result);
}

gchar* tweet_oauth_sign(gchar *access_key,
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
			      CONSUMER_KEY,
			      CONSUMER_SECRET,
			      access_key,
			      access_secret);

  g_free(requesturl);
  return(resulturl);
}

void tweet_oauth_request_token(gchar **request_key,
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
		  CONSUMER_KEY,
		  CONSUMER_SECRET,
		  NULL,
		  NULL);

  response = tweet_curl_http(REQ_TOKEN_URL, postarg, FALSE);
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

gchar* tweet_oauth_gen_authurl(gchar *request_key,
			       gchar *request_secret)
{
  gchar *authurl = NULL;
  gchar *request = NULL;
  gchar *request_key_pair = NULL;
  gchar *request_secret_pair = NULL;

  request = oauth_sign_url2(AUTH_URL,
			    NULL,
			    OA_HMAC,
			    NULL,
			    CONSUMER_KEY,
			    CONSUMER_SECRET,
			    request_key,
			    request_secret);
  request_key_pair = g_strdup_printf("oauth_token=%s", request_key);
  request_secret_pair = g_strdup_printf("oauth_token_secret=%s", request_secret);
  authurl = g_strdup_printf("%s&%s&%s",
			    request,
			    request_key_pair,
			    request_secret_pair);

  g_free(request);
  g_free(request_key_pair);
  g_free(request_secret_pair);

  return(authurl);
}

void tweet_oauth_access_token(gchar *pin,
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
		  CONSUMER_KEY,
		  CONSUMER_SECRET,
		  request_key,
		  request_secret);

  response = tweet_curl_http(ACCESS_TOKEN_URL, postarg, TRUE);
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

void tweet_oauth_access_token_to_file(gchar *access_key,
				      gchar *access_secret)
{
  gchar *access_key_pair = NULL;
  gchar *access_secret_pair = NULL;
  gchar *content = NULL;
  gchar *oauthfile = NULL;

  access_key_pair = g_strdup_printf("oauth_token=%s", access_key);
  access_secret_pair = g_strdup_printf("oauth_token_secret=%s", access_secret);

  content = g_strdup_printf("%s&%s",access_key_pair, access_secret_pair);
  oauthfile = tweet_oauth_expandfilename(OAUTHFILE);
  g_file_set_contents(oauthfile,
		      content,
		      strlen(content),
		      NULL);

  g_free(access_key_pair);
  g_free(access_secret_pair);
  g_free(content);
  g_free(oauthfile);
}

gboolean tweet_oauth_access_token_from_file(gchar **access_key,
					    gchar **access_secret)
{
  gchar *response = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar **token = NULL;
  gboolean result = FALSE;
  gchar *oauthfile = NULL;

  oauthfile = tweet_oauth_expandfilename(OAUTHFILE);

  if(g_file_get_contents(oauthfile, &response, NULL, NULL))
    {
      argc = oauth_split_post_paramters(response, &argv, 0);
      g_free(response);
      while(argc)
	{
	  token = g_strsplit(argv[argc - 1], "=", 0);
	  if(g_strcmp0(token[0], "oauth_token") == 0)
	    *access_key = g_strdup(token[1]);
	  if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	    *access_secret = g_strdup(token[1]);
	  argc--;
	  g_strfreev(token);
	}

      if(access_key == NULL || access_secret == NULL)
	result = FALSE;
      else
	result = TRUE;

      oauth_free_array(&argc, &argv);
    }
  else
    result = FALSE;

  g_free(oauthfile);
  return(result);
}
