#include <tweetpts.h>

#define USERAGENT "Mozilla/5.0 (X11; Linux x86_64; rv:2.0.1) Gecko/20110430 Firefox/4.0.1 Iceweasel/4.0.1"

static size_t write_callback(char *ptr,
			    size_t size,
			    size_t nmemb,
			    void *userdata)
{
  GString *output = (GString *) userdata;
  gsize oldlength = output->len;
  gsize newlength = 0;

  g_string_append(output, ptr);
  newlength = output->len - oldlength;

  return(newlength);
}

void tweetpts_curl(GString *url,
		  GString *post,
		  GString *output)
{
  CURL *handle = curl_easy_init();
  char *effective_url = NULL;
  CURLcode errnum;

  /* curl_easy_setopt(handle, CURLOPT_VERBOSE, 1); */
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)output);
  curl_easy_setopt(handle, CURLOPT_URL, url->str);
  if(post != NULL)
    {
      curl_easy_setopt(handle, CURLOPT_POST, 1);
      curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post->str);
    }
  curl_easy_setopt(handle, CURLOPT_USERAGENT, USERAGENT);

  errnum = curl_easy_perform(handle);
  if(errnum != CURLE_OK)
    {
      g_printerr("%s\n", curl_easy_strerror(errnum));
      exit(1);
    }

  /*
  curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &effective_url);
  g_printerr("effective url: %s\n", effective_url);
  */

  curl_easy_cleanup(handle);
}
