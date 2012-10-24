#ifndef __G_TWEETOBJECT_H__
#define __G_TWEETOBJECT_H__

#include <tweet.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define G_TYPE_TWEETOBJECT (g_tweet_object_get_type())
#define G_TWEETOBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), G_TYPE_TWEETOBJECT, GTweetObject))
#define G_IS_TWEETOBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), G_TYPE_TWEETOBJECT))
#define G_TWEETOBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_TWEETOBJECT, GTweetObjectClass))
#define G_IS_TWEETOBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_TWEETOBJECT))
#define G_TWEETOBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_TWEETOBJECT, GTweetObjectClass))

typedef struct _GTweetObjectClass GTweetObjectClass;
typedef struct _GTweetObject GTweetObject;

struct _GTweetObjectClass
{
  GObjectClass parentClass;
};

struct _GTweetObject
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

GType g_tweet_object_get_type(void) G_GNUC_CONST;

/**
 * g_tweet_object_new:
 */
GTweetObject* g_tweet_object_new(void);

/**
 * g_tweet_object_initkeys:
 */
gboolean g_tweet_object_initkeys(GTweetObject *tweetObject);

/**
 * g_tweet_object_authurl:
 */
gchar* g_tweet_object_authurl(GTweetObject *tweetObject);

/**
 * g_tweet_object_auth:
 */
gboolean g_tweet_object_auth(GTweetObject *tweetObject,
			     gchar *pin);

/**
 * GTweetObjectStreamFunc:
 * @string: tweet data coming from twitter
 * @userdata: user_data provided by the user
 */
typedef gboolean (*GTweetObjectStreamFunc)(gchar *string, gpointer userdata);

/**
 * g_tweet_object_samplestream:
 * @tweetObject: a #TweetObject
 * @func: (closure user_data) (scope async): a callback function to invoke for every tweet
 * @user_data: (closure): data to be sent to the callback.
 */
void g_tweet_object_samplestream(GTweetObject *tweetObject,
				 GTweetObjectStreamFunc func,
				 gpointer user_data);

G_END_DECLS

#endif /* __G_TWEETOBJECT_H__ */
