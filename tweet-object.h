#ifndef __GTWEET_OBJECT_H__
#define __GTWEET_OBJECT_H__

#include <tweet.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GTWEET_TYPE_OBJECT (gtweet_object_get_type())
#define GTWEET_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GTWEET_TYPE_OBJECT, GtweetObject))
#define GTWEET_IS_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTWEET_TYPE_OBJECT))
#define GTWEET_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GTWEET_TYPE_OBJECT, GtweetObjectClass))
#define GTWEET_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTWEET_TYPE_OBJECT))
#define GTWEET_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GTWEET_TYPE_OBJECT, GtweetObjectClass))

typedef struct _GtweetObjectClass GtweetObjectClass;
typedef struct _GtweetObject GtweetObject;

struct _GtweetObjectClass
{
  GObjectClass parentClass;
};

struct _GtweetObject
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

GType gtweet_object_get_type(void) G_GNUC_CONST;

/**
 * gtweet_object_new:
 */
GtweetObject* gtweet_object_new(void);

/**
 * gtweet_object_initkeys:
 */
gboolean gtweet_object_initkeys(GtweetObject *tweetObject);

/**
 * gtweet_object_authurl:
 */
gchar* gtweet_object_authurl(GtweetObject *tweetObject);

/**
 * gtweet_object_auth:
 */
gboolean gtweet_object_auth(GtweetObject *tweetObject,
			     gchar *pin);

/**
 * GtweetObjectStreamFunc:
 * @string: tweet data coming from twitter
 * @userdata: user_data provided by the user
 */
typedef gboolean (*GtweetObjectStreamFunc)(gchar *string, gpointer userdata);

/**
 * gtweet_object_samplestream:
 * @tweetObject: a #TweetObject
 * @func: (closure user_data) (scope async): a callback function to invoke for every tweet
 * @user_data: (closure) (allow-none): data to be sent to the callback.
 */
void gtweet_object_samplestream(GtweetObject *tweetObject,
				GtweetObjectStreamFunc func,
				gpointer user_data);

G_END_DECLS

#endif /* __GTWEET_OBJECT_H__ */
