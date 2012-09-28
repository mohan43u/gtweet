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

gchar* readlineapi_plain(gchar *prompt)
{
  gchar *input = readline(prompt);
  gchar *output = g_strdup(input);
  free(input);
  return(output);
}

static int readlineapi_wgetch_func(FILE *stream)
{
  gchar c;

  noecho();
  c = wgetch(W(inputpanel));
  echo();
  return(c);
}

static void readlineapi_waddch_func(void)
{
  wmove(W(inputpanel), Y(inputpanel), 0);
  waddstr(W(inputpanel), rl_line_buffer);
  wclrtoeol(W(inputpanel));
  wmove(W(inputpanel), Y(inputpanel), rl_point);
  wrefresh(W(inputpanel));
}

static void readlineapi_completion_func(char **matchv, int matchc, int maxstrlen)
{
  guint iter = 0;
  guint Ypos = 0;
  guint Xpos = 0;

  Ypos = Y(inputpanel);
  Xpos = X(inputpanel);
  waddch(W(inputpanel), '[');
  while(matchv[iter])
    {
      waddstr(W(inputpanel), matchv[iter]);
      if(iter < matchc)
	waddstr(W(inputpanel), ", ");
      iter++;
    }
  waddch(W(inputpanel), ']');
  wgetch(W(inputpanel));
  wmove(W(inputpanel), Ypos, Xpos);
  wclrtobot(W(inputpanel));
  wrefresh(W(inputpanel));
}

gchar *readlineapi_wgetstr(void)
{
  rl_getc_func_t *old_getc_func = rl_getc_function;
  rl_voidfunc_t *old_redisplay_func = rl_redisplay_function;
  rl_compdisp_func_t *old_completion_func = rl_completion_display_matches_hook;
  gchar *input = NULL;
  gchar *string = NULL;

  rl_getc_function = readlineapi_wgetch_func;
  rl_redisplay_function = readlineapi_waddch_func;
  rl_completion_display_matches_hook = readlineapi_completion_func;

  input = readline(NULL);
  if(strlen(input))
    add_history(input);
  string = g_strdup(input);
  waddch(W(inputpanel), '\n');
  free(input);

  rl_getc_function = old_getc_func;
  rl_redisplay_function = old_redisplay_func;
  rl_completion_display_matches_hook = old_completion_func;

  return(string);
}
