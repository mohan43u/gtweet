#include <tweetpts.h>

void cursesapi_lock(XPANEL *panel)
{
  if(panel->lockstatus <= 0)
    g_mutex_lock(&(panel->mutex));
  panel->lockstatus += 1;
}

void cursesapi_unlock(XPANEL *panel)
{
  panel->lockstatus -= 1;
  if(panel->lockstatus <= 0)
      g_mutex_unlock(&(panel->mutex));
}

void cursesapi_toggle_lock(XPANEL *panel)
{
  if(panel->lockstatus <= 0)
    cursesapi_lock(panel);
  else
    cursesapi_unlock(panel);
}

void cursesapi_panel_refresh(XPANEL *panel, guint clear)
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

void cursesapi_panel_refresh_all(guint clear)
{
  guint iter = 0;
  while(iter < plist->len)
    {
      XPANEL *panel = g_ptr_array_index(plist, iter);
      cursesapi_panel_refresh(panel, clear);
      iter++;
    }
}

void cursesapi_top(XPANEL *panel)
{
  top_panel(P(panel));
  cursesapi_panel_refresh(panel, 0);
}

XPANEL* cursesapi_panel_new(gushort line,
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
  newpanel->lockstatus = 0;
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

void cursesapi_panel_del(XPANEL *panel)
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

void cursesapi_panel_move(XPANEL *panel, guint y, guint x)
{
  move_panel(P(panel), y, x);
  cursesapi_panel_refresh(panel, 0);
}

void cursesapi_push_string(XPANEL *panel,
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

void cursesapi_push_string_pager(XPANEL *panel, gchar *string)
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

void cursesapi_push_line(XPANEL *panel)
{
  gchar *line = g_strnfill(MX(panel), '-');
  waddch(W(panel), '\n');
  wattrset(W(panel), A_BOLD);
  waddstr(W(panel), line);
  wstandend(W(panel));
  wrefresh(W(panel));
  g_free(line);
}

void cursesapi_push_element(XPANEL *panel, JsonNode *node, gchar *fields)
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

void cursesapi_push_array(XPANEL *panel,
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
      if(prompt)
	cursesapi_push_line(panel);
      if(length - (iter + 1))
	{
	  if(prompt)
	    {
	      infostring = g_strdup_printf("Press 'q' to quit, "
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
	    cursesapi_push_string(panel, "Press any key to finish..", 0);
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

void cursesapi_stream_write(gpointer data, gpointer user_data)
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
	  gchar **fieldsv = g_strsplit(fields, "|", 2);
	  cursesapi_push_element(panel, root, fieldsv[1]);
	  cursesapi_push_line(panel);
	  g_free(fieldsv);
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

gboolean cursesapi_write_cb(GSList *args)
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

void cursesapi_rest_write(gpointer data, gpointer user_data)
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


  cursesapi_lock(tobottom);
  cursesapi_lock(totop);
			
  cursesapi_top(totop);
  cursesapi_panel_refresh(totop, 1);
  if(string && strlen(string))
    {
      JsonParser *parser = jsonapi_parser();
      JsonNode *root = jsonapi_decode(parser, string);
      if(fields && g_strcmp0(fields, "raw") != 0)
	{
	  gchar **fieldsv = NULL;
	  JsonNode *child = NULL;

	  fieldsv = g_strsplit(fields, "|", 2);
	  child = jsonapi_get_object(root, fieldsv[0]);
	  cursesapi_push_array(totop, child, fieldsv[1] , TRUE);
	  g_strfreev(fieldsv);
	  g_object_unref(parser);
	}
      else
	{
	  cursesapi_push_string_pager(totop, string);
	  cursesapi_push_string(totop, "\n", 0);
	  wgetch(W(totop));
	}
      g_free(fields);
    }
  g_free(string);
  g_ptr_array_free(poolargs, FALSE);
  cursesapi_top(tobottom);
  cursesapi_unlock(totop);
  cursesapi_unlock(input);
  cursesapi_unlock(tobottom);
}

void cursesapi_create_baselayout(void)
{
  XPANEL *stdscrpanel = NULL;
  gushort colorpair = 0;

  plist = g_ptr_array_new();
  threadarray = g_ptr_array_new();
  stdscrpanel = g_new0(XPANEL, 1);
  stdscrpanel->panel = new_panel(stdscr);

  userpanel = cursesapi_panel_new(6,
				  XP(stdscrpanel, 20),
				  0,
				  0,
				  colorpair++,
				  COLOR_WHITE,
				  COLOR_CYAN,
				  NULL);
  g_ptr_array_add(plist, userpanel);

  trendspanel = cursesapi_panel_new(6,
				    XP(stdscrpanel, 80) + 1,
				    0,
				    MX(userpanel),
				    colorpair++,
				    COLOR_WHITE,
				    COLOR_MAGENTA,
				    NULL);
  g_ptr_array_add(plist, trendspanel);

  restpanel = cursesapi_panel_new(MY(stdscrpanel) - 10,
				  XP(stdscrpanel, 100),
				  MY(userpanel),
				  0,
				  colorpair++,
				  COLOR_WHITE,
				  COLOR_BLACK,
				  cursesapi_rest_write);
  g_ptr_array_add(plist, restpanel);

  streampanel = cursesapi_panel_new(MY(stdscrpanel) - 10,
				    XP(stdscrpanel, 100),
				    MY(userpanel),
				    0,
				    colorpair++,
				    COLOR_WHITE,
				    COLOR_BLACK,
				    cursesapi_stream_write);
  g_ptr_array_add(plist, streampanel);

  inputpanel = cursesapi_panel_new(2,
				   XP(stdscrpanel, 100),
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
				    MY(userpanel) +
				    MY(restpanel) +
				    MY(inputpanel),
				    0,
				    colorpair++,
				    COLOR_BLACK,
				    COLOR_WHITE,
				    NULL);
  g_ptr_array_add(plist, statuspanel);
  statuspanel->defaultstring = g_strdup("\npress h for help..");
  cursesapi_panel_refresh(statuspanel, 1);

  cursesapi_get_usersettings(userpanel);
  userpanel->defaultfunc = cursesapi_get_usersettings;

  cursesapi_get_trendspanel(trendspanel, NULL);
  g_timeout_add_seconds(TRENDUPDATESEC,
			cursesapi_get_trendspanel_cb,
			trendspanel);

  del_panel(stdscrpanel->panel);
  g_free(stdscrpanel);
}

void cursesapi_destroy_baselayout(void)
{
  guint iter = 0;

  while(iter < plist->len)
    {
      cursesapi_panel_del(g_ptr_array_index(plist, iter));
      iter++;
    }
}

void cursesapi_init(void)
{
  initscr();
  intrflush(stdscr, TRUE);
  keypad(stdscr, TRUE);
  start_color();
  nonl();
  cursesapi_create_baselayout();
}

void cursesapi_free(void)
{
  cursesapi_destroy_baselayout();
  endwin();
}
