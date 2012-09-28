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

gchar* oauthapi_sign(gchar *url, gchar **params, gchar *method)
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

  response = curlapi_http(REQ_TOKEN_URL, postarg, FALSE);
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
    {
      gchar *inputpin = readlineapi_plain("PIN: ");
      pin = g_strdup_printf("oauth_verifier=%s", inputpin);
      g_free(inputpin);
    }

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

  response = curlapi_http(ACCESS_TOKEN_URL, postarg, TRUE);
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
