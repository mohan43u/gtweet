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
#define MYP(panel, p) (YB(panel) + (MY(panel) - YP(panel, p)))
#define MXP(panel, p) (XB(panel) + (MX(panel) - XP(panel, p)))

#define HCHAR '|'
#define VCHAR '-'
#define CURSESAPI_PAIR 1
#define LMAXOFFSET 1
#define CMAXOFFSET 1
#define LASTLINE(w) (MY(w) - LMAXOFFSET)
#define LASTCOL(w) (MX(w) - CMAXOFFSET)

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
};

typedef struct cursesapi_thread
{
  GThread *thread;
  GSList *args;
} XTHREAD;

GPtrArray *plist;

void cursesapi_init(void);
void cursesapi_free(void);
void cursesapi_userinput(GPtrArray *plist);
GPtrArray* cursesapi_create_baselayout(void);
void cursesapi_destroy_baselayout(GPtrArray *plist);


#endif
