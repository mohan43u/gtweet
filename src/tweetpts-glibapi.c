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

static gpointer mainloop_init_cb(gpointer data)
{
  GMainLoop *mainloop = (GMainLoop *) data;
  if(mainloop)
    g_main_loop_run(mainloop);
}

static void iochannel_write_cb(gpointer data, gpointer user_data)
{
  gchar *string = (gchar *) data;

  if(glibapi_iochannel->iochannel)
    {
      g_io_channel_write_chars(glibapi_iochannel->iochannel,
			       string,
			       -1,
			       NULL,
			       NULL);
      g_io_channel_write_chars(glibapi_iochannel->iochannel,
			       "\n",
			       -1,
			       NULL,
			       NULL);
    }
  g_free(string);
}

void glibapi_init(void)
{
  GThread *mainloopthread = NULL;

  g_type_init();
  glibapi_mainloop = g_main_loop_new(NULL, TRUE);
  mainloopthread = g_thread_new("mainloopthread",
				mainloop_init_cb,
				glibapi_mainloop);

  glibapi_iochannel = g_new0(glibapi_giochannel_t, 1);
  glibapi_iochannel->pool = g_thread_pool_new(iochannel_write_cb,
					      glibapi_iochannel,
					      1,
					      TRUE,
					      NULL);
}

void glibapi_free(void)
{
  g_main_loop_quit(glibapi_mainloop);
  glibapi_stop_recording();
  g_thread_pool_free(glibapi_iochannel->pool, TRUE, FALSE);
  g_free(glibapi_iochannel);
}

void glibapi_stop_recording(void)
{
  if(glibapi_iochannel->iochannel)
    {
      g_io_channel_shutdown(glibapi_iochannel->iochannel, TRUE, NULL);
      glibapi_iochannel->iochannel = NULL;
    }
}

void glibapi_start_recording(gchar *filename)
{
  if(glibapi_iochannel->iochannel)
    glibapi_stop_recording();

  glibapi_iochannel->iochannel = g_io_channel_new_file(filename, "w", NULL);
  g_free(filename);
}

void glibapi_write_tweets(gchar *string)
{
  g_thread_pool_push(glibapi_iochannel->pool,
		     g_strdup(string),
		     NULL);
  g_free(string);
}

void glibapi_read_tweets(gchar *fields, gchar *filename, gpointer read_cb)
{
  gboolean (*read_function_cb)(gchar *fields, gchar *string) = read_cb;
  GIOStatus status = G_IO_STATUS_NORMAL;
  GString *buffer = g_string_new(NULL);
  GIOChannel *channel = NULL;

  channel = g_io_channel_new_file(filename, "r", NULL);
  if(channel)
    {
      while(status != G_IO_STATUS_ERROR && status != G_IO_STATUS_EOF)
	{
	  g_string_set_size(buffer, 0);
	  status = g_io_channel_read_line_string(channel,
						 buffer,
						 NULL,
						 NULL);
	  if(buffer->len)
	    if(read_function_cb(fields, g_strdup(buffer->str)) == FALSE)
	      break;
	}
      g_io_channel_shutdown(channel, TRUE, NULL);
    }

  g_string_free(buffer, TRUE);
  g_free(filename);
}

gchar* glibapi_expandfilename(gchar *filename)
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
