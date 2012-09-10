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

typedef struct cursesapi_panel XPANEL;
struct cursesapi_panel
{
  PANEL *panel;
  GMutex mutex;
  GCond wait;
  gint lockstatus;
  GThreadPool *pool;
  void (*defaultfunc)(XPANEL *panel);
  gchar *defaultstring;
  gboolean stopthread;
};

XPANEL *helppanel;
XPANEL *userpanel;
XPANEL *trendspanel;
XPANEL *restpanel;
XPANEL *streampanel;
XPANEL *inputpanel;
XPANEL *statuspanel;
GPtrArray *plist;

typedef struct cursesapi_thread
{
  GThread *thread;
  GSList *args;
} XTHREAD;
GPtrArray *threadarray;

/* defined in tweetpts-cursesapi-display.c */
void cursesapi_lock(XPANEL *panel);
void cursesapi_unlock(XPANEL *panel);
void cursesapi_toggle_lock(XPANEL *panel);
void cursesapi_top(XPANEL *panel);
void cursesapi_panel_refresh(XPANEL *panel, guint clear);
void cursesapi_panel_refresh_all(guint clear);
XPANEL* cursesapi_panel_new(gushort line,
				   gushort column,
				   gushort y,
				   gushort x,
				   gushort colorpair,
				   gushort foreground,
				   gushort background,
				   GFunc poolfunc);
void cursesapi_panel_del(XPANEL *panel);
void cursesapi_panel_move(XPANEL *panel, guint y, guint x);
void cursesapi_push_string(XPANEL *panel,
				  gchar *string,
				  gushort format);
void cursesapi_push_string_pager(XPANEL *panel, gchar *string);
void cursesapi_push_line(XPANEL *panel);
void cursesapi_push_element(XPANEL *panel, JsonNode *node, gchar *fields);
void cursesapi_push_node(XPANEL *panel,
				 JsonNode *root,
				 gchar *fields,
				 gboolean prompt);
void cursesapi_stream_write(gpointer data, gpointer user_data);
gboolean cursesapi_write_cb(GSList *args);
void cursesapi_rest_write(gpointer data, gpointer user_data);
gboolean cursesapi_playback_cb(gchar *fields, gchar *string);
void cursesapi_create_baselayout(void);
void cursesapi_destroy_baselayout(void);
void cursesapi_init(void);
void cursesapi_free(void);

/* defined in tweetpts-cursesapi-keyboard.c */
void cursesapi_call_rest_write(XPANEL *totop,
			       XPANEL *tobottom,
			       XPANEL *input,
			       gchar *inputfields,
			       gchar *inputstring);
void cursesapi_get_usersettings(XPANEL *panel);
void cursesapi_get_trendspanel(XPANEL *panel, gchar *woeid);
gboolean cursesapi_get_trendspanel_cb(gpointer user_data);
void cursesapi_get_trends(gchar *country);
void cursesapi_userinput_thread(gpointer data);
void cursesapi_setfields(guint cmdc, gchar **cmdv);
void cursesapi_filter(guint cmdc, gchar **cmdv);
void cursesapi_sample(guint cmdc, gchar **cmdv);
void cursesapi_firehose(guint cmdc, gchar **cmdv);
void cursesapi_timeline(guint cmdc, gchar **cmdv);
void cursesapi_trends(guint cmdc, gchar **cmdv);
void cursesapi_start_recording(guint cmdc, gchar **cmdv);
void cursesapi_stop_recording(guint cmdc, gchar **cmdv);
void cursesapi_playback(guint cmdc, gchar **cmdv);
void cursesapi_usersettings(guint cmdc, gchar **cmdv);
void cursesapi_space(void);
void cursesapi_finish(void);
void cursesapi_userinput(void);

#endif
