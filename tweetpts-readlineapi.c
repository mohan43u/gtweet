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
      if((iter + 1) < matchc)
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
