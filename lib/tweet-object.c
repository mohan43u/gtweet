/*
 * gtweet - GLib/GObject wrapper for twitter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <tweet.h>

static void gtweet_object_class_init(GtweetObjectClass *klass);
static void gtweet_object_init(GtweetObject *object){}
G_DEFINE_TYPE(GtweetObject, gtweet_object, G_TYPE_OBJECT);

static GObject* gtweet_object_constructor(GType type,
					  guint n_construct_properties,
					  GObjectConstructParam *construct_properties)
{
  GObject *object = NULL;

  object = G_OBJECT_CLASS(gtweet_object_parent_class)->constructor(type,
								   n_construct_properties,
								   construct_properties);
  tweet_soup_init();

  return object;
}

static void gtweet_object_finalize(GObject *object)
{
  GtweetObject *obj = NULL;

  obj = GTWEET_OBJECT(object);
  g_return_if_fail(GTWEET_IS_OBJECT(object));
  if(G_OBJECT_CLASS(gtweet_object_parent_class)->finalize)
    G_OBJECT_CLASS(gtweet_object_parent_class)->finalize(object);

  tweet_soup_free();
}

static void gtweet_object_set_property(GObject *object,
				       guint property_id,
				       const GValue *value,
				       GParamSpec *pspec)
{
  GtweetObject *tweetObject = GTWEET_OBJECT(object);

  g_return_if_fail(GTWEET_IS_OBJECT(object));

  switch(property_id)
    {
    case APPNAME:
      tweetObject->appname = g_value_dup_string(value);
      break;
    case CONSUMER_KEY:
      tweetObject->consumer_key = g_value_dup_string(value);
      break;
    case CONSUMER_SECRET:
      tweetObject->consumer_secret = g_value_dup_string(value);
      break;
    case REQUEST_KEY:
      tweetObject->request_key = g_value_dup_string(value);
      break;
    case REQUEST_SECRET:
      tweetObject->request_secret = g_value_dup_string(value);
      break;
    case ACCESS_KEY:
      tweetObject->access_key = g_value_dup_string(value);
      break;
    case ACCESS_SECRET:
      tweetObject->access_secret = g_value_dup_string(value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void gtweet_object_get_property(GObject *object,
				       guint property_id,
				       GValue *value,
				       GParamSpec *pspec)
{
  GtweetObject *tweetObject = GTWEET_OBJECT(object);

  g_return_if_fail(GTWEET_IS_OBJECT(object));

  switch(property_id)
    {
    case APPNAME:
      g_value_set_string(value, tweetObject->appname);
      break;
    case CONSUMER_KEY:
      g_value_set_string(value, tweetObject->consumer_key);
      break;
    case CONSUMER_SECRET:
      g_value_set_string(value, tweetObject->consumer_secret);
      break;
    case REQUEST_KEY:
      g_value_set_string(value, tweetObject->request_key);
      break;
    case REQUEST_SECRET:
      g_value_set_string(value, tweetObject->request_secret);
      break;
    case ACCESS_KEY:
      g_value_set_string(value, tweetObject->access_key);
      break;
    case ACCESS_SECRET:
      g_value_set_string(value, tweetObject->access_secret);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void gtweet_object_class_init(GtweetObjectClass *klass)
{
  GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);
  gobjectClass->constructor = gtweet_object_constructor;
  gobjectClass->finalize = gtweet_object_finalize;
  gobjectClass->set_property = gtweet_object_set_property;
  gobjectClass->get_property = gtweet_object_get_property;

  g_object_class_install_property(gobjectClass,
				  APPNAME,
				  g_param_spec_string("appname",
						      "application name",
						      "will be stored for reference",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  CONSUMER_KEY,
				  g_param_spec_string("consumer_key",
						      "twitter consumer key",
						      "will be used to generate authurl",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  CONSUMER_SECRET,
				  g_param_spec_string("consumer_secret",
						      "twitter consumer secret",
						      "will be used to generate authurl",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  REQUEST_KEY,
				  g_param_spec_string("request_key",
						      "twitter request key",
						      "will be used to generate authurl",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  REQUEST_SECRET,
				  g_param_spec_string("request_secret",
						      "twitter request secret",
						      "will be used to generate authurl",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  ACCESS_KEY,
				  g_param_spec_string("access_key",
						      "twitter access key",
						      "will be used to sign",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobjectClass,
				  ACCESS_SECRET,
				  g_param_spec_string("access_secret",
						      "twitter access secret",
						      "will be used to sign",
						      NULL,
						      G_PARAM_READABLE|G_PARAM_WRITABLE));
}

GtweetObject* gtweet_object_new(void)
{
  return g_object_new(GTWEET_TYPE_OBJECT, NULL);
}

static void print_properties(GtweetObject *tweetObject)
{
  gchar *appname = NULL;
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  g_object_get(tweetObject,
	       "appname", &appname,
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  g_printerr("appname = %s\n"
	     "consumer_key = %s\n"
	     "consumer_secret = %s\n"
	     "request_key = %s\n"
	     "request_secret = %s\n"
	     "access_key = %s\n"
	     "access_secret = %s\n",
	     appname,
	     consumer_key,
	     consumer_secret,
	     request_key,
	     request_secret,
	     access_key,
	     access_secret);

  g_free(appname);
  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);
}

gboolean gtweet_object_initkeys(GtweetObject *tweetObject)
{
  gchar *appname = NULL;
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gboolean result = FALSE;

  result = tweet_oauth_from_file(&appname,
				 &consumer_key,
				 &consumer_secret,
				 &access_key,
				 &access_secret);
  if(result == TRUE)
    g_object_set(tweetObject,
		 "appname", appname,
		 "consumer_key", consumer_key,
		 "consumer_secret", consumer_secret,
		 "access_key", access_key,
		 "access_secret", access_secret,
		 NULL);

  g_free(appname);
  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  return result;
}

gchar* gtweet_object_gen_authurl(GtweetObject *tweetObject,
				 gchar *consumer_key,
				 gchar *consumer_secret)
{
  gchar *authurl = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;

  tweet_oauth_request_token(consumer_key,
			    consumer_secret,
			    &request_key,
			    &request_secret);
  if(request_key && request_secret)
    {
      g_object_set(G_OBJECT(tweetObject),
		   "consumer_key", consumer_key,
		   "consumer_secret", consumer_secret,
		   "request_key", request_key,
		   "request_secret", request_secret,
		   NULL);
      authurl = tweet_oauth_gen_authurl(consumer_key,
					consumer_secret,
					request_key,
					request_secret);
    }

  g_free(request_key);
  g_free(request_secret);
  return authurl;
}

gboolean gtweet_object_auth(GtweetObject *tweetObject,
			    gchar *appname,
			    gchar *pin)
{
  gboolean result = FALSE;
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       NULL);

  tweet_oauth_access_token(pin,
			   consumer_key,
			   consumer_secret,
			   request_key,
			   request_secret,
			   &access_key,
			   &access_secret);

  if(access_key && access_secret)
    {
      g_object_set(G_OBJECT(tweetObject),
		   "appname", appname,
		   "access_key", access_key,
		   "access_secret", access_secret,
		   NULL);
      tweet_oauth_to_file(appname,
			  consumer_key,
			  consumer_secret,
			  access_key,
			  access_secret);
      result = TRUE;
    }

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_hometimeline(GtweetObject *tweetObject,
				  gchar *count,
				  gchar *since_id,
				  gchar *max_id,
				  gchar *trimuser)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_htimeline(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     count,
				     since_id,
				     max_id,
				     trimuser);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_usertimeline(GtweetObject *tweetObject,
				  gchar *userid,
				  gchar *screenname,
				  gchar *since_id,
				  gchar *count,
				  gchar *max_id)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_utimeline(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     userid,
				     screenname,
				     since_id,
				     count,
				     max_id);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_usersettings(GtweetObject *tweetObject)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_usersettings(consumer_key,
					consumer_secret,
					access_key,
					access_secret);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar *gtweet_object_woeid(GtweetObject *tweetObject,
			   gchar *countryname)
{
  return tweet_twitter_r_woeid(countryname);
}

gchar* gtweet_object_trends(GtweetObject *tweetObject,
			    gchar *woeid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_trends(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  woeid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_tweetsearch(GtweetObject *tweetObject,
				 gchar *q,
				 gchar *geocode,
				 gchar *lang,
				 gchar *locale,
				 gchar *result_type,
				 gchar *count,
				 gchar *until,
				 gchar *since_id,
				 gchar *max_id)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_tweetsearch(consumer_key,
				       consumer_secret,
				       access_key,
				       access_secret,
				       q,
				       geocode,
				       lang,
				       locale,
				       result_type,
				       count,
				       until,
				       since_id,
				       max_id);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_lookup(GtweetObject *tweetObject,
			    gchar *screenname,
			    gchar *user_id)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_lookup(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  screenname,
				  user_id);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_show(GtweetObject *tweetObject,
			  gchar *user_id,
			  gchar *screenname)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_show(consumer_key,
				consumer_secret,
				access_key,
				access_secret,
				user_id,
				screenname);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_usersearch(GtweetObject *tweetObject,
				gchar *q,
				gchar *page,
				gchar *count)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_usersearch(consumer_key,
				      consumer_secret,
				      access_key,
				      access_secret,
				      q,
				      page,
				      count);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_following(GtweetObject *tweetObject,
			       gchar *userid,
			       gchar *screenname,
			       gchar *cursor)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_following(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     userid,
				     screenname,
				     cursor);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_followers(GtweetObject *tweetObject,
			       gchar *userid,
			       gchar *screenname,
			       gchar *cursor)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_followers(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     userid,
				     screenname,
				     cursor);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_updatemedia(GtweetObject *tweetObject,
				 gchar *status,
				 gchar *filepath,
				 gchar *replypostid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_updatemedia(consumer_key,
				       consumer_secret,
				       access_key,
				       access_secret,
				       status,
				       filepath,
				       replypostid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_update(GtweetObject *tweetObject,
			    gchar *status,
			    gchar *replypostid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_update(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  status,
				  replypostid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_retweet(GtweetObject *tweetObject,
			     gchar *postid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_retweet(consumer_key,
				   consumer_secret,
				   access_key,
				   access_secret,
				   postid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_showstatus(GtweetObject *tweetObject,
				gchar *postid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_showstatus(consumer_key,
				      consumer_secret,
				      access_key,
				      access_secret,
				      postid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_destroy(GtweetObject *tweetObject,
			     gchar *postid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_destroy(consumer_key,
				   consumer_secret,
				   access_key,
				   access_secret,
				   postid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_follow(GtweetObject *tweetObject,
			    gchar *screenname,
			    gchar *userid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_follow(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  screenname,
				  userid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_unfollow(GtweetObject *tweetObject,
			      gchar *screenname,
			      gchar *userid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_unfollow(consumer_key,
				    consumer_secret,
				    access_key,
				    access_secret,
				    screenname,
				    userid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_blocklist(GtweetObject *tweetObject,
			       gchar *cursor)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_blocklist(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     cursor);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_block(GtweetObject *tweetObject,
			   gchar *screenname,
			   gchar *userid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_block(consumer_key,
				 consumer_secret,
				 access_key,
				 access_secret,
				 screenname,
				 userid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_unblock(GtweetObject *tweetObject,
			     gchar *screenname,
			     gchar *userid)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_unblock(consumer_key,
				   consumer_secret,
				   access_key,
				   access_secret,
				   screenname,
				   userid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_profile(GtweetObject *tweetObject,
			     gchar *name,
			     gchar *purl,
			     gchar *location,
			     gchar *description)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_profile(consumer_key,
				   consumer_secret,
				   access_key,
				   access_secret,
				   name,
				   purl,
				   location,
				   description);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_pbackground(GtweetObject *tweetObject,
				 gchar *filepath,
				 gchar *tile,
				 gchar *use)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_pbackground(consumer_key,
				       consumer_secret,
				       access_key,
				       access_secret,
				       filepath,
				       tile,
				       use);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

gchar* gtweet_object_pimage(GtweetObject *tweetObject,
			    gchar *filepath)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  result = tweet_twitter_r_pimage(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  filepath);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);

  return result;
}

static gboolean gtweet_generic_base64(gchar *string, gsize length, gpointer user_data)
{
  GPtrArray *cargs = (GPtrArray *) user_data;
  glong fd = (glong) g_ptr_array_index(cargs, 0);
  GCancellable *cancel = g_ptr_array_index(cargs, 1);

  GIOChannel *channel = g_io_channel_unix_new(fd);
  gchar *base64 = g_base64_encode(string, length);
  g_io_channel_write_chars(channel, base64, strlen(base64), NULL, NULL);
  g_io_channel_unref(channel);

  g_free(base64);
  if(g_cancellable_is_cancelled(cancel))
    return FALSE;
  else
    return TRUE;
}

static gpointer samplestream_func(gpointer userdata)
{
  GMainContext *context = NULL;
  GPtrArray *cargs = NULL;
  GtweetObject *tweetObject = NULL;
  glong fd = 0;
  GCancellable *cancel = NULL;

  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GPtrArray *gcargs = NULL;

  context = g_main_context_new();
  g_main_context_push_thread_default(context);

  cargs = (GPtrArray *) userdata;
  tweetObject = g_ptr_array_index(cargs, 0);
  fd = (glong) g_ptr_array_index(cargs, 1);
  cancel = g_ptr_array_index(cargs, 2);

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  gcargs = g_ptr_array_new();
  g_ptr_array_add(gcargs, (gpointer) fd);
  g_ptr_array_add(gcargs, cancel);

  tweet_twitter_s_stat_sample(consumer_key,
			      consumer_secret,
			      access_key,
			      access_secret,
			      gtweet_generic_base64,
			      gcargs);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_ptr_array_free(gcargs, FALSE);
  g_ptr_array_free(cargs, FALSE);
  g_main_context_pop_thread_default(context);
  g_main_context_unref(context);
  g_thread_unref(g_thread_self());
}

void gtweet_object_samplestream(GtweetObject *tweetObject,
				glong fd,
				GCancellable *cancel)
{
  GPtrArray *cargs = g_ptr_array_new();
  g_ptr_array_add(cargs, tweetObject);
  g_ptr_array_add(cargs, (gpointer) fd);
  g_ptr_array_add(cargs, cancel);
  g_thread_new("samplestream", samplestream_func, cargs);
}

static gpointer filterstream_func(gpointer userdata)
{
  GMainContext *context = NULL;
  GPtrArray *cargs = NULL;
  GtweetObject *tweetObject = NULL;
  glong fd = 0;
  GCancellable *cancel = NULL;
  gchar *track = NULL;
  gchar *follow = NULL;
  gchar *locations = NULL;

  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GPtrArray *gcargs = NULL;

  context = g_main_context_new();
  g_main_context_push_thread_default(context);

  cargs = (GPtrArray *) userdata;
  tweetObject = g_ptr_array_index(cargs, 0);
  fd = (glong) g_ptr_array_index(cargs, 1);
  cancel = g_ptr_array_index(cargs, 2);
  track = g_ptr_array_index(cargs, 3);
  follow = g_ptr_array_index(cargs, 4);
  locations = g_ptr_array_index(cargs, 5);

  g_object_get(G_OBJECT(tweetObject),
  	       "consumer_key", &consumer_key,
  	       "consumer_secret", &consumer_secret,
  	       "access_key", &access_key,
  	       "access_secret", &access_secret,
  	       NULL);
  
  gcargs = g_ptr_array_new();
  g_ptr_array_add(gcargs, (gpointer) fd);
  g_ptr_array_add(gcargs, cancel);
  tweet_twitter_s_stat_filter(consumer_key,
			      consumer_secret,
			      access_key,
			      access_secret,
			      track,
			      follow,
			      locations,
			      gtweet_generic_base64,
			      gcargs);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_free(track);
  g_free(follow);
  g_free(locations);
  g_ptr_array_free(gcargs, FALSE);
  g_ptr_array_free(cargs, FALSE);
  g_main_context_pop_thread_default(context);
  g_main_context_unref(context);
  g_thread_unref(g_thread_self());
}

void gtweet_object_filterstream(GtweetObject *tweetObject,
				glong fd,
				GCancellable *cancel,
				gchar *track,
				gchar *follow,
				gchar *locations)
{
  GPtrArray *cargs = g_ptr_array_new();
  g_ptr_array_add(cargs, tweetObject);
  g_ptr_array_add(cargs, (gpointer) fd);
  g_ptr_array_add(cargs, cancel);
  g_ptr_array_add(cargs, g_strdup(track));
  g_ptr_array_add(cargs, g_strdup(follow));
  g_ptr_array_add(cargs, g_strdup(locations));
  g_thread_new("filterstream", filterstream_func, cargs);
}

static gpointer homestream_func(gpointer userdata)
{
  GMainContext *context = NULL;
  GPtrArray *cargs = NULL;
  GtweetObject *tweetObject = NULL;
  glong fd = 0;
  GCancellable *cancel = NULL;
  gchar *track = NULL;
  gchar *locations = NULL;

  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GPtrArray *gcargs = NULL;

  context = g_main_context_new();
  g_main_context_push_thread_default(context);

  cargs = (GPtrArray *) userdata;
  tweetObject = g_ptr_array_index(cargs, 0);
  fd = (glong) g_ptr_array_index(cargs, 1);
  cancel = g_ptr_array_index(cargs, 2);
  track = g_ptr_array_index(cargs, 3);
  locations = g_ptr_array_index(cargs, 4);

  g_object_get(G_OBJECT(tweetObject),
  	       "consumer_key", &consumer_key,
  	       "consumer_secret", &consumer_secret,
  	       "access_key", &access_key,
  	       "access_secret", &access_secret,
  	       NULL);
  
  gcargs = g_ptr_array_new();
  g_ptr_array_add(gcargs, (gpointer) fd);
  g_ptr_array_add(gcargs, cancel);
  tweet_twitter_s_htimeline(consumer_key,
  			    consumer_secret,
  			    access_key,
  			    access_secret,
  			    track,
  			    locations,
  			    gtweet_generic_base64,
  			    gcargs);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_free(track);
  g_free(locations);
  g_ptr_array_free(gcargs, FALSE);
  g_ptr_array_free(cargs, FALSE);
  g_main_context_pop_thread_default(context);
  g_main_context_unref(context);
  g_thread_unref(g_thread_self());
}

void gtweet_object_homestream(GtweetObject *tweetObject,
			      glong fd,
			      GCancellable *cancel,
			      gchar *track,
			      gchar *locations)
{
  GPtrArray *cargs = g_ptr_array_new();
  g_ptr_array_add(cargs, tweetObject);
  g_ptr_array_add(cargs, (gpointer) fd);
  g_ptr_array_add(cargs, cancel);
  g_ptr_array_add(cargs, g_strdup(track));
  g_ptr_array_add(cargs, g_strdup(locations));
  g_thread_new("homestream", homestream_func, cargs);
}

static gpointer httpfunc(gpointer userdata)
{
  GMainContext *context = NULL;
  GPtrArray *cargs = (GPtrArray *) userdata;
  gchar *url = (gchar *) g_ptr_array_index(cargs, 0);
  glong fd = (glong) g_ptr_array_index(cargs, 1);

  context = g_main_context_new();
  g_main_context_push_thread_default(context);
  
  GString *buffer = tweet_soup_gstring_sync(url, NULL, FALSE);
  GIOChannel *channel = g_io_channel_unix_new(fd);
  gchar *base64 = g_base64_encode(buffer->str, buffer->len);
  g_io_channel_write_chars(channel, base64, strlen(base64), NULL, NULL);
  g_io_channel_unref(channel);

  g_free(base64);
  g_string_free(buffer, TRUE);
  g_free(url);
  g_ptr_array_free(cargs, FALSE);
  g_main_context_pop_thread_default(context);
  g_main_context_unref(context);
  g_thread_unref(g_thread_self());
}

void gtweet_object_http(GtweetObject *tweetObject, gchar *url, glong fd)
{
  GPtrArray *cargs = g_ptr_array_new();
  g_ptr_array_add(cargs, g_strdup(url));
  g_ptr_array_add(cargs, (gpointer) fd);
  g_thread_new("http", httpfunc, cargs);
}

gint* gtweet_object_pipe(GtweetObject *tweetObject)
{
  gint *fds = g_new0(gint, 2);
  if(g_unix_open_pipe(fds, 0, NULL))
    return fds;
  else
    return NULL;
}

GByteArray* gtweet_object_read_base64(GtweetObject *tweetObject,
				      GInputStream *inputStream,
				      GCancellable *cancel)
{
  GString *response = g_string_new(NULL);
  GByteArray *result = g_byte_array_new();
  gsize length = 0;
  gchar *base64 = NULL;

  while(TRUE)
    {
      gchar *buffer = g_strnfill(4096, '\0');
      gsize length = 0;
      length = g_input_stream_read(inputStream,
				   buffer,
				   4096,
				   cancel,
				   NULL);
      g_string_append_len(response, buffer, length);
      g_free(buffer);
      if(length != 4096)
	break;
    }
  base64 = g_base64_decode(response->str, &length);
  g_byte_array_append(result, base64, length);

  g_free(base64);
  g_string_free(response, TRUE);
  return result;
}

gchar* gtweet_object_read(GtweetObject *tweetObject,
			  GInputStream *inputStream,
			  GCancellable *cancel)
{
  GByteArray *bytes = gtweet_object_read_base64(tweetObject, inputStream, cancel);
  GString *string = g_string_new_len(bytes->data, bytes->len);
  gchar *result = g_strdup(string->str);

  g_byte_array_free(bytes, TRUE);
  g_string_free(string, TRUE);
  return result;
}
