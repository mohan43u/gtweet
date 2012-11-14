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
  const gchar *consumer_key;
  const gchar *consumer_secret;
  const gchar *request_key;
  const gchar *request_secret;
  const gchar *access_key;
  const gchar *access_secret;
};

enum
  {
    PROP_0,
    CONSUMER_KEY,
    CONSUMER_SECRET,
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
 * gtweet_object_gen_authurl:
 */
gchar* gtweet_object_gen_authurl(GtweetObject *tweetObject,
				 gchar *consumer_key,
				 gchar *consumer_secret);

/**
 * gtweet_object_auth:
 */
gboolean gtweet_object_auth(GtweetObject *tweetObject,
			     gchar *pin);

/**
 * gtweet_object_hometimeline:
 * @count: (allow-none): number of tweets to get
 * @since_id: (allow-none): tweets to retrive since since_id
 * @max_id: (allow-none): tweets to retrive from max_id
 */
gchar* gtweet_object_hometimeline(GtweetObject *tweetObject,
				  gchar *count,
				  gchar *since_id,
				  gchar *max_id);

/**
 * gtweet_object_usertimeline:
 */
gchar* gtweet_object_usertimeline(GtweetObject *tweetObject,
				  gchar *userid,
				  gchar *screenname,
				  gchar *since_id,
				  gchar *count,
				  gchar *max_id);

/**
 * gtweet_object_usersettings:
 */
gchar* gtweet_object_usersettings(GtweetObject *tweetObject);

/**
 * gtweet_object_trends:
 */
gchar* gtweet_object_trends(GtweetObject *tweetObject,
			    gchar *countryname);

/**
 * gtweet_object_tweetsearch:
 */
gchar* gtweet_object_tweetsearch(GtweetObject *tweetObject,
				 gchar *q,
				 gchar *geocode,
				 gchar *lang,
				 gchar *locale,
				 gchar *result_type,
				 gchar *count,
				 gchar *until,
				 gchar *since_id,
				 gchar *max_id);

/**
 * gtweet_object_lookup:
 */
gchar* gtweet_object_lookup(GtweetObject *tweetObject,
			    gchar *screenname,
			    gchar *user_id);

/**
 * gtweet_object_usersearch:
 */
gchar* gtweet_object_usersearch(GtweetObject *tweetObject,
				gchar *q,
				gchar *page,
				gchar *count);

/**
 * gtweet_object_following:
 */
gchar* gtweet_object_following(GtweetObject *tweetObject,
			       gchar *userid,
			       gchar *screenname,
			       gchar *cursor);

/**
 * gtweet_object_followers:
 */
gchar* gtweet_object_followers(GtweetObject *tweetObject,
			       gchar *userid,
			       gchar *screenname,
			       gchar *cursor);

/**
 * gtweet_object_updatemedia:
 */
gchar* gtweet_object_updatemedia(GtweetObject *tweetObject,
				 gchar *status,
				 gchar *filepath);

/**
 * gtweet_object_update:
 */
gchar* gtweet_object_update(GtweetObject *tweetObject,
			    gchar *status,
			    gchar *replypostid);

/**
 * gtweet_object_retweet:
 */
gchar* gtweet_object_retweet(GtweetObject *tweetObject,
			     gchar *postid);

/**
 * gtweet_object_destroy:
 */
gchar* gtweet_object_destroy(GtweetObject *tweetObject,
			     gchar *postid);

/**
 * gtweet_object_follow:
 */
gchar* gtweet_object_follow(GtweetObject *tweetObject,
			    gchar *screenname,
			    gchar *userid);

/**
 * gtweet_object_unfollow:
 */
gchar* gtweet_object_unfollow(GtweetObject *tweetObject,
			      gchar *screenname,
			      gchar *userid);

/**
 * gtweet_object_blocklist:
 */
gchar* gtweet_object_blocklist(GtweetObject *tweetObject,
			       gchar *cursor);

/**
 * gtweet_object_block:
 */
gchar* gtweet_object_block(GtweetObject *tweetObject,
			   gchar *screenname,
			   gchar *userid);

/**
 * gtweet_object_unblock:
 */
gchar* gtweet_object_unblock(GtweetObject *tweetObject,
			     gchar *screenname,
			     gchar *userid);

/**
 * gtweet_object_profile:
 */
gchar* gtweet_object_profile(GtweetObject *tweetObject,
			     gchar *name,
			     gchar *purl,
			     gchar *location,
			     gchar *description);

/**
 * gtweet_object_pbackground:
 */
gchar* gtweet_object_pbackground(GtweetObject *tweetObject,
				 gchar *filepath,
				 gchar *tile,
				 gchar *use);

/**
 * gtweet_object_pimage:
 */
gchar* gtweet_object_pimage(GtweetObject *tweetObject,
			    gchar *filepath);

/**
 * gtweet_object_samplestream:
 * @tweetObject: a #TweetObject
 * @cancel: a cancellable object
 * @callback: (closure user_data) (scope async): a callback function to invoke for every tweet
 * @user_data: (closure) (allow-none): data to be sent to the callback.
 */
void gtweet_object_samplestream(GtweetObject *tweetObject,
				GCancellable *cancel,
				GAsyncReadyCallback callback,
				gpointer user_data);

/**
 * gtweet_object_filterstream:
 * @tweetObject: a #TweetObject
 * @cancel: a cancellable object
 * @callback: (closure user_data) (scope async): a callback function to invoke for every tweet
 * @user_data: (closure) (allow-none): data to be sent to the callback
 * @track: string used to filter tweets
 * @follow: userid used to filter tweets
 * @locations: location string used to filter tweets
 */

void gtweet_object_filterstream(GtweetObject *tweetObject,
				GCancellable *cancel,
				GAsyncReadyCallback callback,
				gpointer user_data,
				gchar *track,
				gchar *follow,
				gchar *locations);

/**
 * gtweet_object_homestream:
 * @tweetObject: a #TweetObject
 * @cancel: a cancellable object
 * @callback: (closure user_data) (scope async): a callback function to invoke for every tweet
 * @user_data: (closure) (allow-none): data to be sent to the callback
 * @track: string used to filter tweets
 * @locations: location string used to filter tweets
 */
void gtweet_object_homestream(GtweetObject *tweetObject,
			      GCancellable *cancel,
			      GAsyncReadyCallback callback,
			      gpointer user_data,
			      gchar *track,
			      gchar *locations);

G_END_DECLS

#endif /* __GTWEET_OBJECT_H__ */
