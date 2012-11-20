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
    case STREAM_RESPONSE:
	tweetObject->stream_response = g_value_dup_string(value);
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
    case STREAM_RESPONSE:
	g_value_take_string(value, tweetObject->stream_response);
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
  g_object_class_install_property(gobjectClass,
				  STREAM_RESPONSE,
				  g_param_spec_string("stream_response",
						     "http response chunk",
						     "will be used on stream api calls",
						     NULL,
						     G_PARAM_READABLE|G_PARAM_WRITABLE));
}

GtweetObject* gtweet_object_new(void)
{
  return g_object_new(GTWEET_TYPE_OBJECT, NULL);
}

static void print_properties(GtweetObject *tweetObject)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *stream_response = NULL;

  g_object_get(tweetObject,
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       "stream_response", &stream_response,
	       NULL);

  g_printerr("consumer_key = %s\n"
	     "consumer_secret = %s\n"
	     "request_key = %s\n"
	     "request_secret = %s\n"
	     "access_key = %s\n"
	     "access_secret = %s\n",
	     consumer_key,
	     consumer_secret,
	     request_key,
	     request_secret,
	     access_key,
	     access_secret);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);
}

gboolean gtweet_object_initkeys(GtweetObject *tweetObject)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gboolean result = FALSE;

  result = tweet_oauth_from_file(&consumer_key,
				 &consumer_secret,
				 &access_key,
				 &access_secret);
  if(result == TRUE)
    g_object_set(tweetObject,
		 "consumer_key", consumer_key,
		 "consumer_secret", consumer_secret,
		 "access_key", access_key,
		 "access_secret", access_secret,
		 NULL);

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
		   "access_key", access_key,
		   "access_secret", access_secret,
		   NULL);
      tweet_oauth_to_file(consumer_key,
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

  result = tweet_twitter_r_htimeline(consumer_key,
				     consumer_secret,
				     access_key,
				     access_secret,
				     count,
				     since_id,
				     max_id);

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

gchar* gtweet_object_trends(GtweetObject *tweetObject,
			    gchar *countryname)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gchar *woeid = NULL;
  gchar *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  woeid = tweet_twitter_r_woeid(countryname);
  result = tweet_twitter_r_trends(consumer_key,
				  consumer_secret,
				  access_key,
				  access_secret,
				  woeid);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_free(woeid);

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

  result = tweet_twitter_r_updatemedia(consumer_key,
				       consumer_secret,
				       access_key,
				       access_secret,
				       status,
				       filepath);

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

static gboolean gtweet_generic_callback(gchar *string, gsize length, gpointer user_data)
{
  GSimpleAsyncResult *result = (GSimpleAsyncResult *) user_data;
  GCancellable *cancel = g_simple_async_result_get_source_tag(result);
  GObject *tweetObject = g_async_result_get_source_object(G_ASYNC_RESULT(result));

  g_object_set(tweetObject,
	       "stream_response", string,
	       NULL);
  g_simple_async_result_complete(result);

  if(!g_cancellable_is_cancelled(cancel))
    return TRUE;
  else
    return FALSE;
}

void gtweet_object_samplestream(GtweetObject *tweetObject,
				GCancellable *cancel,
				GAsyncReadyCallback callback,
				gpointer user_data)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GSimpleAsyncResult *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);
  result = g_simple_async_result_new(G_OBJECT(tweetObject),
				     callback,
				     user_data,
				     cancel);
  g_simple_async_result_set_check_cancellable(result,
					      cancel);
  tweet_twitter_s_stat_sample(consumer_key,
			      consumer_secret,
			      access_key,
			      access_secret,
			      gtweet_generic_callback,
			      result);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_object_unref(result);
}

void gtweet_object_filterstream(GtweetObject *tweetObject,
				GCancellable *cancel,
				GAsyncReadyCallback callback,
				gpointer user_data,
				gchar *track,
				gchar *follow,
				gchar *locations)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GSimpleAsyncResult *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);
  result = g_simple_async_result_new(G_OBJECT(tweetObject),
				     callback,
				     user_data,
				     cancel);
  g_simple_async_result_set_check_cancellable(result,
					      cancel);

  tweet_twitter_s_stat_filter(consumer_key,
  			      consumer_secret,
  			      access_key,
  			      access_secret,
  			      track,
  			      follow,
  			      locations,
  			      gtweet_generic_callback,
  			      result);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_object_unref(result);
}

void gtweet_object_homestream(GtweetObject *tweetObject,
			      GCancellable *cancel,
			      GAsyncReadyCallback callback,
			      gpointer user_data,
			      gchar *track,
			      gchar *locations)
{
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  GSimpleAsyncResult *result = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);
  result = g_simple_async_result_new(G_OBJECT(tweetObject),
				     callback,
				     user_data,
				     cancel);
  g_simple_async_result_set_check_cancellable(result,
					      cancel);
  tweet_twitter_s_htimeline(consumer_key,
			    consumer_secret,
			    access_key,
			    access_secret,
			    track,
			    locations,
			    gtweet_generic_callback,
			    result);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(access_key);
  g_free(access_secret);
  g_object_unref(result);
}

GByteArray* gtweet_object_http(GtweetObject *tweetObject,
			       gchar *url)
{
  GString *buffer = tweet_soup_gstring_sync(url, NULL, FALSE);
  GByteArray *array = g_byte_array_new_take(buffer->str, buffer->len);
  return array;
}
