#ifndef __TWEET_OBJECT_H__
#define __TWEET_OBJECT_H__

#include <tweet.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TWEET_TYPE_OBJECT (tweet_object_get_type())
#define TWEET_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TWEET_TYPE_OBJECT, TweetObject))
#define TWEET_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TWEET_TYPE_OBJECT))
#define TWEET_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TWEET_TYPE_OBJECT, TweetObjectClass))
#define TWEET_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TWEET_TYPE_OBJECT))
#define TWEET_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TWEET_TYPE_OBJECT, TweetObjectClass))

typedef struct _TweetObjectClass TweetObjectClass;
typedef struct _TweetObject TweetObject;

struct _TweetObjectClass
{
  GObjectClass parentClass;
};

struct _TweetObject
{
  GObject parent;

  /*<private>*/
  const gchar *request_key;
  const gchar *request_secret;
  const gchar *access_key;
  const gchar *access_secret;
};

enum
  {
    PROP_0,
    REQUEST_KEY,
    REQUEST_SECRET,
    ACCESS_KEY,
    ACCESS_SECRET
  };

GType tweet_object_get_type(void) G_GNUC_CONST;

TweetObject* tweet_object_new(void);
gboolean tweet_object_initkeys(TweetObject *tweetObject);
gchar* tweet_object_authurl(TweetObject *tweetObject);
gboolean tweet_object_auth(TweetObject *tweetObject,
			   gchar *pin);

typedef gboolean (*TweetObjectStreamFunc)(gchar *string, gpointer userdata);
void tweet_object_samplestream(TweetObject *tweetObject,
			       TweetObjectStreamFunc func,
			       gpointer userdata);

G_END_DECLS

#endif /* __TWEET_OBJECT_H__ */
