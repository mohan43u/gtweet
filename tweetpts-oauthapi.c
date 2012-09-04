#include <tweetpts.h>

gchar* oauthapi_sign(gchar *url,
		   gchar **params)
{
  gchar *signed_url = NULL;
  gchar *request = NULL;

  if(params)
    request = g_strdup_printf("%s?%s", url, *params);
  else
    request = g_strdup(url);

  signed_url = oauth_sign_url2(request,
			       params,
			       OA_HMAC,
			       NULL,
			       CONSUMER_KEY,
			       CONSUMER_SECRET,
			       access_key,
			       access_secret);
  g_free(request);
  return(signed_url);
}

void oauthapi_request_token(void)
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
  g_free(request);

  response = curlapi_http(REQ_TOKEN_URL, postarg);
  g_free(postarg);

  argc = oauth_split_post_paramters(response, &argv, 0);
  g_free(response);

  while(argc)
    {
      token = g_strsplit(argv[argc - 1], "=", 0);
      if(g_strcmp0(token[0], "oauth_token") == 0)
	{
	  request_key_pair = g_strdup(argv[argc - 1]);
	  request_key = g_strdup(token[1]);
	}
      if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	{
	  request_secret_pair = g_strdup(argv[argc - 1]);
	  request_secret = g_strdup(token[1]);
	}
      argc--;
      g_strfreev(token);
    }
  oauth_free_array(&argc, &argv);
}

void oauthapi_authenticate(void)
{
  gchar *cmdline = NULL;
  gchar *request = NULL;

  request = oauth_sign_url2(AUTH_URL,
		  NULL,
		  OA_HMAC,
		  NULL,
		  CONSUMER_KEY,
		  CONSUMER_SECRET,
		  request_key,
		  request_secret);
  cmdline = g_strdup_printf(CMDLINE_FMT,
			    request,
			    request_key_pair,
			    request_secret_pair);
  g_free(request);

  if(g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL))
    pin = g_strdup_printf("oauth_verifier=%s", readline("PIN: "));
  g_free(cmdline);
}

void oauthapi_access_token(void)
{
  gchar *request = NULL;
  gchar *response = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar *postarg = NULL;
  gchar **token = NULL;

  request = g_strdup_printf("%s?%s", ACCESS_TOKEN_URL, pin);
  oauth_sign_url2(request,
		  &postarg,
		  OA_HMAC,
		  NULL,
		  CONSUMER_KEY,
		  CONSUMER_SECRET,
		  request_key,
		  request_secret);
  g_free(request);

  response = curlapi_http(ACCESS_TOKEN_URL, postarg);
  g_free(postarg);

  argc = oauth_split_post_paramters(response, &argv, 0);
  g_free(response);

  while(argc)
    {
      token = g_strsplit(argv[argc - 1], "=", 0);
      if(g_strcmp0(token[0], "oauth_token") == 0)
	{
	  access_key_pair = g_strdup(argv[argc - 1]);
	  access_key = g_strdup(token[1]);
	}
      if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	{
	  access_secret_pair = g_strdup(argv[argc - 1]);
	  access_secret = g_strdup(token[1]);
	}
      argc--;
      g_strfreev(token);
    }
  if(access_key == NULL || access_secret == NULL)
    {
      g_print("things went wrong!! not able to get access_{key,secret}!!\n");
      oauthapi_free();
      exit(EXIT_FAILURE);
    }
  oauth_free_array(&argc, &argv);
}

void oauthapi_access_token_to_file(void)
{
  gchar *content = g_strdup_printf("%s&%s",access_key_pair, access_secret_pair);
  g_file_set_contents(OAUTHFILE,
		      content,
		      strlen(content),
		      NULL);
  g_free(content);
}

void oauthapi_access_token_from_file(void)
{
  gchar *response = NULL;
  gint argc = 0;
  gchar **argv = NULL;
  gchar **token = NULL;

  if(g_file_get_contents(OAUTHFILE, &response, NULL, NULL))
    {
      argc = oauth_split_post_paramters(response, &argv, 0);
      g_free(response);
      while(argc)
	{
	  token = g_strsplit(argv[argc - 1], "=", 0);
	  if(g_strcmp0(token[0], "oauth_token") == 0)
	    {
	      access_key_pair = g_strdup(argv[argc - 1]);
	      access_key = g_strdup(token[1]);
	    }
	  if(g_strcmp0(token[0], "oauth_token_secret") == 0)
	    {
	      access_secret_pair = g_strdup(argv[argc - 1]);
	      access_secret = g_strdup(token[1]);
	    }
	  argc--;
	  g_strfreev(token);
	}
      if(access_key == NULL || access_secret == NULL)
	{
	  g_print("things went wrong!! not able to get access_{key,secret}!!\n");
	  oauthapi_free();
	  exit(EXIT_FAILURE);
	}
      oauth_free_array(&argc, &argv);
    }
  else
    {
      oauthapi_request_token();
      oauthapi_authenticate();
      oauthapi_access_token();
      oauthapi_access_token_to_file();
    }
}

gchar* expandfilename(gchar *filename)
{
  wordexp_t result_t;
  gchar *result = NULL;
  if(wordexp(filename, &result_t, 0) == 0)
    result = g_strdup(result_t.we_wordv[result_t.we_wordc - 1]);
  else
    result = g_strdup(filename);
  wordfree(&result_t);
  return(result);
}
      

void oauthapi_init(void)
{
  oauthapi_access_token_from_file();
}

void oauthapi_free(void)
{
  g_free(request_key_pair);
  g_free(request_secret_pair);
  g_free(request_key);
  g_free(request_secret);
  g_free(access_key_pair);
  g_free(access_secret_pair);
  g_free(access_key);
  g_free(access_secret);
  g_free(pin);
}
