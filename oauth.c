#include <tweetpts.h>

#define CON_KEY "Y9nGZBw5oH7oOpxReTpAg"
#define CON_SECRET "6LIalH3UA2F2fgefBRbBfXLcWKG03TeBlk7FvuBxjAQ"
#define CON_REQ_TOK_URL "https://api.twitter.com/oauth/request_token"
#define CON_AUTH_URL "https://api.twitter.com/oauth/authorize"
#define CON_AC_TOK_URL "https://api.twitter.com/oauth/access_token"
#define OAUTH_KEYS_FILE "~/.tweetpts"

void tweetpts_gen_url(GString *url,
		      GString *parameters,
		      const gchar *http_method,
		      GString *req_token,
		      GString *req_secret,
		      GString *signed_url)
{
  gchar *l_signed_url = NULL;
  gchar *l_parameters = NULL;

  l_signed_url = oauth_sign_url2(url->str,
				 &l_parameters,
				 OA_HMAC,
				 http_method,
				 CON_KEY,
				 CON_SECRET,
				 req_token->str,
				 req_secret->str);
  g_string_assign(signed_url, l_signed_url);
  g_string_assign(parameters, l_parameters);

  g_free(l_parameters);
  g_free(l_signed_url);
}

static void gen_keys(GString *output,
		     GString *token,
		     GString *secret)
{
  char **keys = NULL;
  char **token_splitted = NULL;
  char **secret_splitted = NULL;
  int num_keys = 0;
  int num_token_splitted = 2;
  int num_secret_splitted = 2;

  num_keys = oauth_split_post_paramters(output->str, &keys, 0);

  token_splitted = g_strsplit(keys[0], "=", 0); /* first key must be token */
  secret_splitted = g_strsplit(keys[1], "=", 0); /* second key must be secret */

  g_string_assign(token, token_splitted[1]); /* assigning only 'value' not 'token=value' */
  g_string_assign(secret, secret_splitted[1]); /* assigning only 'value' not 'secret=value' */

  /* free temporary places */
  oauth_free_array(&num_keys, &keys);
  oauth_free_array(&num_token_splitted, &token_splitted);
  oauth_free_array(&num_secret_splitted, &secret_splitted);
}

static void get_pin(GString *con_auth_url,
		    GString *request_token,
		    GString *pin)
{
  char *userinput = NULL;

  g_print("Open following URL and authorize tweetpts to access your account.\n");
  g_print("authorize URL : %s?oauth_token=%s\n",
	  con_auth_url->str,
	  request_token->str);
  g_print("Once authorized, twitter will provide you a PIN, type that PIN here.\n");
  userinput = readline("PIN: ");
  g_string_assign(pin, userinput);

  g_free(userinput);
}

static void expand_filename(GString *oauth_keys_file)
{
  wordexp_t result;
  wordexp(oauth_keys_file->str, &result, 0);
  g_string_assign(oauth_keys_file, result.we_wordv[0]);
  wordfree(&result);
}

static void gen_access_keys(GString *access_token,
			    GString *access_secret)
{
  GString *con_req_tok_url = g_string_new(CON_REQ_TOK_URL);
  GString *con_auth_url = g_string_new(CON_AUTH_URL);
  GString *con_ac_tok_url = g_string_new(CON_AC_TOK_URL);
  GString *oauth_keys_file = g_string_new(OAUTH_KEYS_FILE);
  GString *oauth_keys_file_content = g_string_new(NULL);
  GString *signed_con_req_tok_url = g_string_new(NULL);
  GString *post = g_string_new(NULL);
  GString *output = g_string_new(NULL);
  GString *request_token = g_string_new(NULL);
  GString *request_secret = g_string_new(NULL);
  GString *pin = g_string_new(NULL);
  GString *signed_con_ac_tok_url = g_string_new(NULL);
  GString *post2 = g_string_new(NULL);
  GString *output2 = g_string_new(NULL);
  GError *error = NULL;
  gboolean returnvalue;


  tweetpts_gen_url(con_req_tok_url, post, "POST", request_token, request_secret, signed_con_req_tok_url);
  tweetpts_curl(signed_con_req_tok_url, post, output);
  gen_keys(output, request_token, request_secret);
  get_pin(con_auth_url, request_token, pin);
  g_string_append_printf(con_ac_tok_url, "/?oauth_verifier=%s",pin->str);
  tweetpts_gen_url(con_ac_tok_url, post2, "POST", request_token, request_secret, signed_con_ac_tok_url);
  tweetpts_curl(signed_con_ac_tok_url, post2, output2);
  gen_keys(output2, access_token, access_secret);


  g_string_append_printf(oauth_keys_file_content, "access_token=%s\n", access_token->str);
  g_string_append_printf(oauth_keys_file_content, "access_secret=%s\n", access_secret->str);
  g_string_append_printf(oauth_keys_file_content, "request_url=%s\n", signed_con_req_tok_url->str);
  g_string_append_printf(oauth_keys_file_content, "post=%s\n", post->str);
  g_string_append_printf(oauth_keys_file_content, "output=%s\n", output->str);
  g_string_append_printf(oauth_keys_file_content, "request_token=%s\n", request_token->str);
  g_string_append_printf(oauth_keys_file_content, "request_secret=%s\n", request_secret->str);
  g_string_append_printf(oauth_keys_file_content, "pin=%s\n", pin->str);
  g_string_append_printf(oauth_keys_file_content, "access_url=%s\n", signed_con_ac_tok_url->str);
  g_string_append_printf(oauth_keys_file_content, "post2=%s\n", post2->str);
  g_string_append_printf(oauth_keys_file_content, "output2=%s\n", output2->str);

  expand_filename(oauth_keys_file);
  returnvalue = g_file_set_contents(oauth_keys_file->str,
				    oauth_keys_file_content->str,
				    oauth_keys_file_content->len,
				    &error);
  if(returnvalue == FALSE)
    {
      g_printerr("%s\n", error->message);
      exit(1);
    }

  g_string_free(con_req_tok_url, TRUE);
  g_string_free(con_auth_url, TRUE);
  g_string_free(con_ac_tok_url, TRUE);
  g_string_free(oauth_keys_file, TRUE);
  g_string_free(oauth_keys_file_content, TRUE);
  g_string_free(signed_con_req_tok_url, TRUE);
  g_string_free(post, TRUE);
  g_string_free(output, TRUE);
  g_string_free(request_token, TRUE);
  g_string_free(request_secret, TRUE);
  g_string_free(signed_con_ac_tok_url, TRUE);
  g_string_free(post2, TRUE);
  g_string_free(output2, TRUE);
}

static void get_access_keys(GString *oauth_keys_file,
			    GString *access_token,
			    GString *access_secret)
{
  GString *output = g_string_new(NULL);
  gchar *content = NULL;
  gchar **content_array = NULL;
  gchar *newcontent = NULL;
  gsize length = 0;
  GError *error = NULL;
  gboolean returncode;
  gint content_array_length = 11; /* we know it, because we the one who saved in gen_access_keys() */

  returncode = g_file_get_contents(oauth_keys_file->str,
				   &content,
				   &length,
				   &error);
  if(returncode == FALSE)
    {
      g_printerr("%s\n", error->message);
      exit(1);
    }

  content_array = g_strsplit(content, "\012", 0); /*taking only first 2 keys */
  newcontent = g_strjoin("&", content_array[0], content_array[1], NULL); /* converting into POST variables */
  g_string_assign(output, newcontent);

  gen_keys(output, access_token, access_secret);

  g_free(content);
  oauth_free_array(&content_array_length, &content_array);
  g_free(newcontent);
}

void tweetpts_oauth(GString *access_token,
		    GString *access_secret)
{
  GString *oauth_keys_file = g_string_new(OAUTH_KEYS_FILE);

  expand_filename(oauth_keys_file);
  if(g_file_test(oauth_keys_file->str, G_FILE_TEST_IS_REGULAR) == TRUE)
    get_access_keys(oauth_keys_file, access_token, access_secret);
  else
    gen_access_keys(access_token, access_secret);

  g_string_free(oauth_keys_file, TRUE);
}
