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

#ifndef __TWEETPTS_CURSESAPI__
#define __TWEETPTS_CURSESAPI__

#include <curses.h>
#include <panel.h>

#define P(p) (p->panel)
#define W(panel) panel_window(P(panel))
#define YP(panel, p) ((p * getmaxy(W(panel)))/100)
#define XP(panel, p) ((p * getmaxx(W(panel)))/100)
#define MY(panel) getmaxy(W(panel))
#define MX(panel) getmaxx(W(panel))
#define YB(panel) getbegy(W(panel))
#define XB(panel) getbegx(W(panel))
#define Y(panel) getcury(W(panel))
#define X(panel) getcurx(W(panel))

#define BUFFERSIZE 4098
#define CURSESAPI_PAIR 1
#define FIELDWIDTH 12
#define VALUE_LINE_BEG(panel) (FIELDWIDTH)
#define VALUE_LINE_LEN(panel) (MX(panel) - FIELDWIDTH)

typedef struct cursesapi_panel cursesapi_panel_t;
struct cursesapi_panel
{
  PANEL *panel;
  GMutex mutex;
  GCond wait;
  gint lockstatus;
  GThreadPool *pool;
  void (*defaultfunc)(cursesapi_panel_t *panel);
  gchar *defaultstring;
  gboolean stopthread;
};

#define TRENDUPDATESEC (5*60)
cursesapi_panel_t *helppanel;
cursesapi_panel_t *userpanel;
cursesapi_panel_t *trendspanel;
cursesapi_panel_t *streampanel;
cursesapi_panel_t *inputpanel;
cursesapi_panel_t *statuspanel;
GPtrArray *plist;
gchar *cursesapi_woeid;


/* defined in tweetpts-cursesapi-keyboard.c */
void cursesapi_userinput(void);
void cursesapi_get_usersettings(cursesapi_panel_t *panel);
gboolean cursesapi_get_trendspanel_cb(gpointer user_data);
void cursesapi_get_trendspanel(cursesapi_panel_t *panel, gchar *woeid);

/* defined in tweetpts-cursesapi-display.c */
void cursesapi_create_baselayout(void);
void cursesapi_destroy_baselayout(void);
void cursesapi_init(void);
void cursesapi_free(void);
gboolean cursesapi_playback_cb(gchar *fields, gchar *string);
gboolean cursesapi_write_cb(gchar *string, gpointer userdata);
void cursesapi_lock(cursesapi_panel_t *panel);
void cursesapi_unlock(cursesapi_panel_t *panel);
void cursesapi_toggle_lock(cursesapi_panel_t *panel);
void cursesapi_panel_refresh(cursesapi_panel_t *panel, guint clear);
void cursesapi_panel_refresh_all(guint clear);
void cursesapi_top(cursesapi_panel_t *panel);
cursesapi_panel_t* cursesapi_panel_new(gushort line, gushort column, gushort y,
				       gushort x, gushort colorpair,
				       gushort foreground, gushort background,
				       GFunc poolfunc);
void cursesapi_panel_del(cursesapi_panel_t *panel);
void cursesapi_panel_move(cursesapi_panel_t *panel, guint y, guint x);
void cursesapi_push_string(cursesapi_panel_t *panel, gchar *string, 
			   gushort format);
void cursesapi_push_string_pager(cursesapi_panel_t *panel, gchar *string);
void cursesapi_push_line(cursesapi_panel_t *panel);
void cursesapi_push_element(cursesapi_panel_t *panel, JsonNode *node,
			    gchar *fields);
void cursesapi_push_node(cursesapi_panel_t *panel, JsonNode *root, gchar *fields,
			 gboolean prompt);
void cursesapi_stream_write(gpointer data, gpointer user_data);
void cursesapi_rest_write(cursesapi_panel_t *panel, cursesapi_panel_t *input,
			  gchar *fields, gchar *string);

#endif
