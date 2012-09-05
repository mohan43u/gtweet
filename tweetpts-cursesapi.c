#include <tweetpts.h>

static void cursesapi_set_pause(XPANEL *panel);
static void cursesapi_remove_pause(XPANEL *panel);
static void cursesapi_toggle_pause(XPANEL *panel);
static void cursesapi_top(XPANEL *panel);
static void cursesapi_panel_refresh(XPANEL *panel, guint clear);
static void cursesapi_panel_refresh_all(GPtrArray *plist, guint clear);
static XPANEL* cursesapi_panel_new(gushort line,
				   gushort column,
				   gushort y,
				   gushort x,
				   gushort colorpair,
				   gushort foreground,
				   gushort background,
				   GFunc poolfunc);
static void cursesapi_panel_del(XPANEL *panel);
static void cursesapi_panel_move(XPANEL *panel, guint y, guint x);
static void cursesapi_push_string(XPANEL *panel,
				  gchar *string,
				  gushort format);
static void cursesapi_push_string_pager(XPANEL *panel, gchar *string);
static void cursesapi_push_element(XPANEL *panel, JsonNode *node, gchar *fields);
static void cursesapi_push_array(XPANEL *panel,
				 JsonNode *root,
				 gchar *fields,
				 gboolean prompt);
static void cursesapi_stream_write(gpointer data, gpointer user_data);
static gboolean cursesapi_write_cb(GSList *args);
static void cursesapi_rest_write(gpointer data, gpointer user_data);
static void cursesapi_usertrends(XPANEL *panel, gchar *woeid);
static gboolean cursesapi_usertrends_cb(gpointer user_data);
static void cursesapi_usersettings(XPANEL *panel);
static void cursesapi_get_trends(XPANEL *totop,
				 XPANEL *tobottom,
				 XPANEL *input,
				 gchar *country);
static void cursesapi_userinput_thread(gpointer data);

void cursesapi_init(void)
{
  initscr();
  intrflush(stdscr, TRUE);
  keypad(stdscr, TRUE);
  start_color();
  nonl();
}

void cursesapi_free(void)
{
  endwin();
}

static void cursesapi_set_pause(XPANEL *panel)
{
  if(panel->lockstatus == FALSE)
    {
      g_mutex_lock(&(panel->mutex));
      panel->lockstatus = TRUE;
    }
}

static void cursesapi_remove_pause(XPANEL *panel)
{
  if(panel->lockstatus == TRUE)
    {
      panel->lockstatus = FALSE;
      g_mutex_unlock(&(panel->mutex));
    }
}

static void cursesapi_toggle_pause(XPANEL *panel)
{
  if(panel->lockstatus == FALSE)
    cursesapi_set_pause(panel);
  else
    cursesapi_remove_pause(panel);
}

static void cursesapi_panel_refresh(XPANEL *panel, guint clear)
{
  if(clear == 1)
    {
      wclear(W(panel));
      wattrset(W(panel), A_BOLD);
      waddstr(W(panel), panel->defaultstring);
      wstandend(W(panel));
      if(panel->defaultfunc)
	panel->defaultfunc(panel);
    }
  if(clear == 2)
    redrawwin(W(panel));
  wrefresh(W(panel));
  update_panels();
}

static void cursesapi_panel_refresh_all(GPtrArray *plist, guint clear)
{
  guint iter = 0;
  while(iter < plist->len)
    {
      XPANEL *panel = g_ptr_array_index(plist, iter);
      cursesapi_panel_refresh(panel, clear);
      iter++;
    }
}

static void cursesapi_top(XPANEL *panel)
{
  top_panel(P(panel));
  cursesapi_panel_refresh(panel, 0);
}

static XPANEL* cursesapi_panel_new(gushort line,
				   gushort column,
				   gushort y,
				   gushort x,
				   gushort colorpair,
				   gushort foreground,
				   gushort background,
				   GFunc poolfunc)
{
  WINDOW *newwindow = NULL;
  XPANEL *newpanel = NULL;

  newpanel = g_new0(XPANEL, 1);
  g_mutex_init(&(newpanel->mutex));
  g_cond_init(&(newpanel->wait));
  newpanel->lockstatus = FALSE;
  newpanel->stopthread = FALSE;
  if(poolfunc)
    newpanel->pool = g_thread_pool_new((GFunc) poolfunc,
				       newpanel,
				       1,
				       TRUE,
				       NULL);
  newwindow = newwin(line, column, y, x);
  newpanel->panel = new_panel(newwindow);
  scrollok(newwindow, TRUE);
  colorpair = (COLOR_PAIRS - 1) - colorpair;
  if(colorpair <= CURSESAPI_PAIR)
    colorpair = colorpair + 1;
  init_pair(colorpair, foreground, background);
  wcolor_set(newwindow, colorpair, NULL);
  wbkgd(newwindow, COLOR_PAIR(colorpair));
  cursesapi_panel_refresh(newpanel, 0);

  return(newpanel);
}

static void cursesapi_panel_del(XPANEL *panel)
{
  if(panel->pool)
    g_thread_pool_free(panel->pool, TRUE, FALSE);
  g_cond_clear(&(panel->wait));
  g_mutex_clear(&(panel->mutex));
  if(panel->defaultstring)
    g_free(panel->defaultstring);
  panel->stopthread = TRUE;
  del_panel(P(panel));
  delwin(W(panel));
  g_free(panel);
}

static void cursesapi_panel_move(XPANEL *panel, guint y, guint x)
{
  move_panel(P(panel), y, x);
  cursesapi_panel_refresh(panel, 0);
}

static void cursesapi_push_string(XPANEL *panel,
				  gchar *string,
				  gushort format)
{
  if(format == 0)
    waddstr(W(panel), string);
  if(format == 1)
    {
      gchar *fstring = g_strdup_printf("%*s", FIELDWIDTH, string);
      wmove(W(panel), Y(panel), 0);
      wrefresh(W(panel));
      waddstr(W(panel), fstring);
      g_free(fstring);
    }
  if(format == 2)
    {
      guint valuelength =  0;
      guint pos = 0;
      guint length = 0;
      gunichar *ucs4string = NULL;
      wchar_t *newlinechar = NULL;
      gchar **stringv = NULL;
      gchar *jstring = NULL;

      stringv = g_strsplit(string, "\n", 0);
      jstring = g_strjoinv(" ", stringv);
      valuelength = VALUE_LINE_LEN(panel)  - 2;
      
      ucs4string = g_utf8_to_ucs4_fast(jstring, -1, NULL);
      length = wcslen(ucs4string);
      if(length > valuelength)
	{
	  while(pos < length)
	    {
	      gunichar *line = NULL;
	      line = g_new0(gunichar, valuelength + 1);
	      wcsncpy(line, &(ucs4string[pos]), valuelength);
	      line[valuelength] = L'\0';
	      newlinechar = line;
	      pos = pos + (valuelength);
	      wmove(W(panel), Y(panel), VALUE_LINE_BEG(panel));
	      wrefresh(W(panel));
	      waddwstr(W(panel), line);
	      if(pos < length)
		waddwstr(W(panel), L"\n");
	      g_free(line);
	    }
	}
      else
	{
	  wmove(W(panel), Y(panel), VALUE_LINE_BEG(panel));
	  wrefresh(W(panel));
	  ucs4string = g_utf8_to_ucs4_fast(jstring, -1, NULL);
	  waddwstr(W(panel), ucs4string);
	  g_free(ucs4string);
	}
    }

  cursesapi_panel_refresh(panel, 0);
}

static void cursesapi_push_string_pager(XPANEL *panel, gchar *string)
{
  if(string && strlen(string))
    {
      guint iter = 0;
      guint length = strlen(string);
      while(iter < length)
	{
	  waddch(W(panel), string[iter]);
	  wrefresh(W(panel));
	  if((Y(panel) + 1) == MY(panel))
	    {
	      wmove(W(panel), Y(panel), 0);
	      wrefresh(W(panel));
	      waddstr(W(panel), "Press any key to continue..");
	      noecho();
	      wgetch(W(panel));
	      flushinp();
	      echo();
	      wscrl(W(panel), MY(panel) / 2);
	      wmove(W(panel), MY(panel) / 2, 0);
	      wrefresh(W(panel));
	    }
	  iter++;
	}
    }
}

static void cursesapi_push_element(XPANEL *panel, JsonNode *node, gchar *fields)
{
  if(fields && JSON_NODE_HOLDS_OBJECT(node))
    {
      gchar **fieldarray = g_strsplit(fields, ",", 0);
      guint iter = 0;
      gchar *line = NULL;

      while(fieldarray[iter])
	{
	  gchar *value = NULL;
	  gchar *outfield = NULL;
	  gchar **fielddict = NULL;

	  fielddict = g_strsplit(fieldarray[iter], "|", 2);
	  if(fielddict[1])
	    {
	      value = jsonapi_get_value(node, fielddict[1]);
	      if(value && strlen(value))
		{
		  outfield = g_strdup_printf("%s ", fielddict[0]);
		  g_strfreev(fielddict);

		  wattrset(W(panel), A_BOLD);
		  cursesapi_push_string(panel, outfield, 1);
		  wstandend(W(panel));
		  g_free(outfield);

		  cursesapi_push_string(panel, value, 2);
		  if(fieldarray[iter + 1])
		    cursesapi_push_string(panel, "\n", 0);
		}
	      g_free(value);
	    }
	  iter++;
	}
    }
}

static void cursesapi_push_array(XPANEL *panel,
				 JsonNode *root,
				 gchar *fields,
				 gboolean prompt)
{
  guint length = jsonapi_length(root);
  guint iter = 0;
  gchar *infostring = NULL;

  while(iter < length)
    {
      JsonNode *node = jsonapi_get_element(root, iter);

      cursesapi_push_element(panel, node, fields);
      cursesapi_push_string(panel, "\n", 0);
      if(length - (iter + 1))
	{
	  if(prompt)
	    {
	      infostring = g_strdup_printf("\nPress 'q' to quit, "
					   "any other key to contine, "
					   "%d remaining to show..",
					   length - (iter + 1));
	      cursesapi_push_string(panel, infostring, 0);
	      g_free(infostring);
	    }
	}
      else
	{
	  if(prompt)
	    cursesapi_push_string(panel, "\nPress any key to finish..", 0);
	}

      if(prompt)
	if(wgetch(W(panel)) == 'q')
	  {
	    flushinp();
	    break;
	  }

      iter++;
    }
}

static void cursesapi_stream_write(gpointer data, gpointer user_data)
{
  XPANEL *panel = NULL;
  GPtrArray *poolargs = NULL;
  gchar *fields = NULL;
  gchar *string = NULL;

  panel = (XPANEL *) user_data;

  g_mutex_lock(&(panel->mutex));
  poolargs = (GPtrArray *) data;
  fields = g_ptr_array_index(poolargs, 0);
  string = g_ptr_array_index(poolargs, 1);
  if(string && strlen(string))
    {
      if(string[0] == '{' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);
	  cursesapi_push_element(panel, root, fields);
	  cursesapi_push_string(panel, "\n\n\n", 0);
	  g_object_unref(parser);
	}
      else
	{
	  cursesapi_push_string_pager(panel, string);
	  cursesapi_push_string(panel, "\n", 0);
	}
    }
  g_ptr_array_free(poolargs, TRUE);

  g_mutex_unlock(&(panel->mutex));
}

static gboolean cursesapi_write_cb(GSList *args)
{
  XPANEL *panel = (g_slist_nth(args, 0))->data;
  gchar *fields = (g_slist_nth(args, 1))->data;
  gchar *string = (g_slist_nth(args, 2))->data;

  // someone asked me to die! ok, done..
  if(panel->stopthread == TRUE)
    {
      panel->stopthread = FALSE;
      return(FALSE);
    }

  GPtrArray *poolargs = g_ptr_array_new();
  g_ptr_array_add(poolargs, g_strdup(fields));
  g_ptr_array_add(poolargs, g_strdup(string));
  g_thread_pool_push(panel->pool, poolargs, NULL);

  return(TRUE);
}

static void cursesapi_rest_write(gpointer data, gpointer user_data)
{
  XPANEL *totop = NULL;
  XPANEL *tobottom = NULL;
  XPANEL *input = NULL;
  GPtrArray *poolargs = NULL;
  gchar *fields = NULL;
  gchar *string = NULL;

  totop = (XPANEL *) user_data;
  
  poolargs = (GPtrArray *) data;
  tobottom = g_ptr_array_index(poolargs, 0);
  input = g_ptr_array_index(poolargs, 1);
  fields = g_ptr_array_index(poolargs, 2);
  string = g_ptr_array_index(poolargs, 3);


  cursesapi_set_pause(input);
  cursesapi_set_pause(tobottom);
  cursesapi_set_pause(totop);
			
  cursesapi_top(totop);
  cursesapi_panel_refresh(totop, 1);
  cursesapi_push_string(totop,
			"streaming has been stopped.."
			"You have to resume after finishing this view..\n\n",
			0);
  if(string && strlen(string))
    {
      if(string[0] == '[' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);
	  cursesapi_push_array(totop, root, fields, TRUE);
	  g_object_unref(parser);
	}
      else
	{
	  cursesapi_push_string(totop, string, 0);
	  wgetch(W(totop));
	  flushinp();
	}
    }
  g_free(fields);
  g_free(string);
  g_ptr_array_free(poolargs, FALSE);
  cursesapi_top(tobottom);
  cursesapi_remove_pause(totop);
  cursesapi_remove_pause(input);
}

static void cursesapi_usersettings(XPANEL *panel)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  if(twitterapi_userfields->fields->len)
     fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_USERSETTINGS_FIELD);

  string = twitterapi_get_usersettings();
  if(string && strlen(string))
    {
      if(string[0] == '{' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);
	  gchar *woeid = NULL;

	  cursesapi_push_element(panel, root, fields);
	  if(twitterapi_woeid->fields->len == 0)
	    {
	      woeid = jsonapi_get_value(root, ".trend_location[*].woeid|int");
	      g_mutex_lock(&(twitterapi_woeid->mutex));
	      g_string_assign(twitterapi_woeid->fields, woeid);
	      g_mutex_unlock(&(twitterapi_woeid->mutex));
	    }
	  g_object_unref(parser);
	}
      else
	{
	  top_panel(P(panel));
	  cursesapi_push_string(panel, string, 0);
	}
    }
  g_free(fields);
  g_free(string);
}

static void cursesapi_usertrends(XPANEL *panel, gchar *woeid)
{
  gchar *fields = NULL;
  gchar *string = NULL;

  g_mutex_lock(&(panel->mutex));

  if(twitterapi_userfields->fields->len)
     fields = twitterapi_get_userfields();
  else
    fields = g_strdup(T_TRENDS_FIELD);

  string = twitterapi_get_usertrends(woeid);
  if(string && strlen(string))
    {
      if(string[0] == '[' && fields && g_strcmp0("raw", fields) != 0)
	{
	  JsonParser *parser = jsonapi_parser();
	  JsonNode *root = jsonapi_decode(parser, string);

	  cursesapi_panel_refresh(panel, 1);
	  cursesapi_push_array(panel, root, fields, FALSE);
	  g_object_unref(parser);
	}
      else
	{
	  top_panel(P(panel));
	  cursesapi_push_string(panel, string, 0);
	}
    }
  g_free(fields);
  g_free(string);

  g_mutex_unlock(&(panel->mutex));
}

static gboolean cursesapi_usertrends_cb(gpointer user_data)
{
  XPANEL *panel = (XPANEL *) user_data;
  cursesapi_usertrends(panel, NULL);
  return(TRUE);
}

static void cursesapi_get_trends(XPANEL *totop,
				 XPANEL *tobottom,
				 XPANEL *input,
				 gchar *country)
{
  gchar *woeid = NULL;
  gchar **woeidv = NULL;
  guint iter = 0;
  GPtrArray *poolargs = g_ptr_array_new();

  woeid = twitterapi_get_woeid(country);
  if(woeid && strlen(woeid) && g_str_has_prefix(woeid, "url=") != TRUE)
    {
      woeidv = g_strsplit(woeid, "|", 0);
      while(woeidv[iter])
	{
	  gchar *fields = NULL;
	  gchar *string = NULL;
	  gchar *swoeid = g_strstrip(woeidv[iter]);

	  if(twitterapi_userfields->fields->len)
	    fields = twitterapi_get_userfields();
	  else
	    fields = g_strdup(T_TRENDS_FIELD);

	  g_mutex_lock(&(twitterapi_woeid->mutex));
	  g_string_assign(twitterapi_woeid->fields, swoeid);
	  g_mutex_unlock(&(twitterapi_woeid->mutex));

	  string = twitterapi_get_usertrends(swoeid);
	  if(string && strlen(string))
	    {
	      if(g_str_has_prefix(string, "CURL") == TRUE)
		{
		  g_free(fields);
		  fields = g_strdup("raw");
		}
	      g_ptr_array_add(poolargs, tobottom);
	      g_ptr_array_add(poolargs, input);
	      g_ptr_array_add(poolargs, fields);
	      g_ptr_array_add(poolargs, string);
	      g_thread_pool_push(totop->pool, poolargs, NULL);
	    }
	  iter++;
	}
      g_strfreev(woeidv);
    }
  else
    {
      gchar *fields = g_strdup("raw");
      gchar *string = g_strdup(woeid);
      g_ptr_array_add(poolargs, tobottom);
      g_ptr_array_add(poolargs, input);
      g_ptr_array_add(poolargs, fields);
      g_ptr_array_add(poolargs, string);
      g_thread_pool_push(totop->pool, poolargs, NULL);
    }

  g_free(woeid);
}

static void cursesapi_userinput_thread(gpointer data)
{
  GSList *args = (GSList *) data;
  void (*twitterapi_function)(GSList *args) = (g_slist_nth(args, 0))->data;
  GSList *threadargs = g_slist_copy(g_slist_nth(args, 1));
  twitterapi_function(threadargs);
  g_slist_free(threadargs);
}

void cursesapi_userinput(GPtrArray *plist)
{
  XPANEL *helppanel = g_ptr_array_index(plist, 0);
  XPANEL *restpanel = g_ptr_array_index(plist, 1);
  XPANEL *streampanel = g_ptr_array_index(plist, 2);
  XPANEL *userpanel = g_ptr_array_index(plist, 3);
  XPANEL *trendspanel = g_ptr_array_index(plist, 4);
  XPANEL *inputpanel = g_ptr_array_index(plist, 5);
  XPANEL *statuspanel = g_ptr_array_index(plist, 6);
  GPtrArray *threadarray = g_ptr_array_new();
  guint iter = 0;

  while(true)
    {
      gchar input;

      noecho();
      flushinp();
      g_mutex_lock(&(inputpanel->mutex));
      input = wgetch(W(inputpanel));
      g_mutex_unlock(&(inputpanel->mutex));

      if(input == 'c')
	{
	  gchar cmd[BUFFERSIZE];
	  wordexp_t cmdexp;

	  cursesapi_push_string(statuspanel, "\ncommandline mode..", 0);
	  cursesapi_top(inputpanel);
	  echo();
	  flushinp();
	  wgetstr(W(inputpanel), cmd);
	  if(wordexp(cmd, &cmdexp, 0) == 0)
	    {
	      gchar **cmdv = cmdexp.we_wordv;
	      guint cmdc = cmdexp.we_wordc;

	      if(g_strcmp0("setfields", cmdv[0]) == 0)
		if(cmdc >= 2)
		  twitterapi_set_userfields(cmdv[1]);

	      if(g_strcmp0("filter", cmdv[0]) == 0)
		{
		  GSList *args = NULL;
		  XTHREAD *thread = NULL;
		  gchar *track = NULL;
		  gchar *follow = NULL;
		  gchar *locations = NULL;

		  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
		  follow = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
		  locations = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

		  args = g_slist_append(args, twitterapi_s_stat_filter);
		  args = g_slist_append(args, track);
		  args = g_slist_append(args, follow);
		  args = g_slist_append(args, locations);
		  args = g_slist_append(args, cursesapi_write_cb);
		  args = g_slist_append(args, streampanel);
		  thread = g_new0(XTHREAD, 1);
		  thread->thread = g_thread_new("filter",
						(GThreadFunc)
						cursesapi_userinput_thread,
						args);
		  thread->args = args;
		  g_ptr_array_add(threadarray, thread);
		}

	      if(g_strcmp0("sample", cmdv[0]) == 0)
		{
		  GSList *args = NULL;
		  XTHREAD *thread = NULL;

		  args = g_slist_append(args, twitterapi_s_stat_sample);
		  args = g_slist_append(args, cursesapi_write_cb);
		  args = g_slist_append(args, streampanel);
		  thread = g_new0(XTHREAD, 1);
		  thread->thread = g_thread_new("sample",
						(GThreadFunc)
						cursesapi_userinput_thread,
						args);
		  thread->args = args;
		  g_ptr_array_add(threadarray, thread);
		}

	      if(g_strcmp0("firehose", cmdv[0]) == 0)
		{
		  GSList *args = NULL;
		  XTHREAD *thread = NULL;
		  gchar *count = NULL;

		  count = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);

		  args = g_slist_append(args, twitterapi_s_stat_firehose);
		  args = g_slist_append(args, count);
		  args = g_slist_append(args, cursesapi_write_cb);
		  args = g_slist_append(args, streampanel);
		  thread = g_new0(XTHREAD, 1);
		  thread->thread = g_thread_new("firehose",
						(GThreadFunc)
						cursesapi_userinput_thread,
						args);
		  thread->args = args;
		  g_ptr_array_add(threadarray, thread);
		}

	      if(g_strcmp0("timelinestream", cmdv[0]) == 0 ||
		 g_strcmp0("timeline", cmdv[0]) == 0)
		{
		  GSList *args = NULL;
		  XTHREAD *thread = NULL;
		  gchar *timeline = NULL;
		  gchar *track = NULL;
		  gchar *locations = NULL;
		  gchar *count = NULL;

		  track = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
		  locations = (cmdc >= 3 ? g_strdup(cmdv[2]) : NULL);
		  count = (cmdc >= 4 ? g_strdup(cmdv[3]) : NULL);

		  timeline = twitterapi_r_timeline(count);
		  if(timeline && strlen(timeline))
		    {
		      GPtrArray *poolargs = g_ptr_array_new();
		      gchar *fields = NULL;
		      gchar *string = NULL;

		      if(twitterapi_userfields->fields->len)
			fields = twitterapi_get_userfields();
		      else
			fields = g_strdup(T_TIMELINE_FIELD);
		      string = g_strdup(timeline);
		      g_free(timeline);

		      g_ptr_array_add(poolargs, streampanel);
		      g_ptr_array_add(poolargs, inputpanel);
		      g_ptr_array_add(poolargs, fields);
		      g_ptr_array_add(poolargs, string);
		      g_thread_pool_push(restpanel->pool, poolargs, NULL);
		      cursesapi_push_string(statuspanel,
					    "\nstreaming paused..",
					    0);
		    }

		  if(g_strcmp0("timelinestream", cmdv[0]) == 0)
		    {
		      args = g_slist_append(args, twitterapi_s_timeline);
		      args = g_slist_append(args, track);
		      args = g_slist_append(args, locations);
		      args = g_slist_append(args, cursesapi_write_cb);
		      args = g_slist_append(args, streampanel);
		      thread = g_new0(XTHREAD, 1);
		      thread->thread = g_thread_new("timeline",
						    (GThreadFunc)
						    cursesapi_userinput_thread,
						    args);
		      thread->args = args;
		      g_ptr_array_add(threadarray, thread);
		    }
		}

	      if(g_strcmp0("trends", cmdv[0]) == 0)
		{
		  gchar *country = NULL;
		  country = (cmdc >= 2 ? g_strdup(cmdv[1]) : NULL);
		  cursesapi_get_trends(restpanel,
				       streampanel,
				       inputpanel,
				       country);
		  cursesapi_usertrends(trendspanel, NULL);
		  cursesapi_push_string(statuspanel, "\nstreaming paused..", 0);
		}

	      if(g_strcmp0("usersettings", cmdv[0]) == 0)
		cursesapi_usersettings(userpanel);

	      cursesapi_push_string(statuspanel, "\nleaving commandline..", 0);
	      wordfree(&cmdexp);
	    }
	}

      if(input == ' ')
	{
	  cursesapi_toggle_pause(streampanel);
	  if(streampanel->lockstatus == TRUE)
	    cursesapi_push_string(statuspanel, "\nstreaming paused..", 0);
	  else
	    cursesapi_push_string(statuspanel, "\nstreaming resumed..", 0);
	}

      if(input == 'f')
	{
	  streampanel->stopthread = TRUE;
	  cursesapi_push_string(statuspanel, "\nstreaming closed..", 0);
	}

      if(input == 'r')
	cursesapi_panel_refresh_all(plist, 2);

      if(input == 'l')
	cursesapi_panel_refresh_all(plist, 1);

      if(input == 'q')
	break;
    }

  while(iter < threadarray->len)
    {
      XTHREAD *thread = g_ptr_array_index(threadarray, iter);
      cursesapi_set_pause(streampanel);
      cursesapi_set_pause(restpanel);
      g_slist_free(thread->args);
      cursesapi_remove_pause(restpanel);
      cursesapi_remove_pause(streampanel);
      iter++;
    }
  g_ptr_array_free(threadarray, FALSE);
}

GPtrArray* cursesapi_create_baselayout(void)
{
  XPANEL *stdscrpanel = NULL;
  XPANEL *helppanel = NULL;
  XPANEL *userpanel = NULL;
  XPANEL *trendspanel = NULL;
  XPANEL *restpanel = NULL;
  XPANEL *streampanel = NULL;
  XPANEL *inputpanel = NULL;
  XPANEL *statuspanel = NULL;
  gushort colorpair = 0;

  plist = g_ptr_array_new();
  stdscrpanel = g_new0(XPANEL, 1);
  stdscrpanel->panel = new_panel(stdscr);

  helppanel = cursesapi_panel_new(2,
				  XP(stdscrpanel, 100),
				  0,
				  0,
				  colorpair++,
				  COLOR_WHITE,
				  COLOR_GREEN,
				  NULL);
  g_ptr_array_add(plist, helppanel);
  helppanel->defaultstring = g_strdup("c=(commandmode)"
				      " [space]=(pause/resume streaming)"
				      " f=(finish streaming)"
				      " r=(refresh)"
				      " l=(clear)"
				      " q=(quit)");
  cursesapi_panel_refresh(helppanel, 1);

  restpanel = cursesapi_panel_new(MY(stdscrpanel) - 12,
				  XP(stdscrpanel, 100),
				  MY(helppanel),
				  0,
				  colorpair++,
				  COLOR_WHITE,
				  COLOR_BLACK,
				  cursesapi_rest_write);
  g_ptr_array_add(plist, restpanel);

  streampanel = cursesapi_panel_new(MY(stdscrpanel) - 12,
				    XP(stdscrpanel, 100),
				    MY(helppanel),
				    0,
				    colorpair++,
				    COLOR_WHITE,
				    COLOR_BLACK,
				    cursesapi_stream_write);
  g_ptr_array_add(plist, streampanel);

  userpanel = cursesapi_panel_new(6,
				  XP(stdscrpanel, 20),
				  MY(helppanel) + MY(restpanel),
				  0,
				  colorpair++,
				  COLOR_WHITE,
				  COLOR_CYAN,
				  NULL);
  g_ptr_array_add(plist, userpanel);

  trendspanel = cursesapi_panel_new(6,
				    XP(stdscrpanel, 80) + 1,
				    MY(helppanel) + MY(restpanel),
				    MX(userpanel),
				    colorpair++,
				    COLOR_WHITE,
				    COLOR_MAGENTA,
				    NULL);
  g_ptr_array_add(plist, trendspanel);

  inputpanel = cursesapi_panel_new(2,
				   XP(stdscrpanel, 100),
				   MY(helppanel) +
				   MY(userpanel) +
				   MY(restpanel),
				   0,
				   colorpair++,
				   COLOR_WHITE,
				   COLOR_BLUE,
				   NULL);
  g_ptr_array_add(plist, inputpanel);

  statuspanel = cursesapi_panel_new(2,
				    XP(stdscrpanel, 100),
				    MY(helppanel) +
				    MY(userpanel) +
				    MY(restpanel) +
				    MY(inputpanel),
				    0,
				    colorpair++,
				    COLOR_WHITE,
				    COLOR_GREEN,
				    NULL);
  g_ptr_array_add(plist, statuspanel);

  cursesapi_usersettings(userpanel);
  userpanel->defaultfunc = cursesapi_usersettings;

  cursesapi_usertrends(trendspanel, NULL);
  g_timeout_add_seconds(TRENDUPDATESEC,
			cursesapi_usertrends_cb,
			trendspanel);

  del_panel(stdscrpanel->panel);
  g_free(stdscrpanel);
  return(plist);
}

void cursesapi_destroy_baselayout(GPtrArray *plist)
{
  guint iter = 0;

  while(iter < plist->len)
    {
      cursesapi_panel_del(g_ptr_array_index(plist, iter));
      iter++;
    }
}
