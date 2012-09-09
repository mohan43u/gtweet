#ifndef __TWEETPTS_GLIBAPI__
#define __TWEETPTS_GLIBAPI__

typedef struct
{
  GIOChannel *iochannel;
  GThreadPool *pool;
  GMutex mutex;
} XGIOChannel;

GMainLoop *glibapi_mainloop;
XGIOChannel *glibapi_iochannel;

void glibapi_init(void);
void glibapi_free(void);
void glibapi_stop_recording(void);
void glibapi_start_recording(gchar *filename);
void glibapi_write_tweets(gchar *string);
void glibapi_read_tweets(gchar *fields, gchar *filename, gpointer read_cb);

#endif
