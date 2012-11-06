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
  return object;
}

static void gtweet_object_finalize(GObject *object)
{
  GtweetObject *obj = NULL;
  gchar *consumer_key = NULL;
  gchar *consumer_secret = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  obj = GTWEET_OBJECT(object);
  g_return_if_fail(GTWEET_IS_OBJECT(object));
  if(G_OBJECT_CLASS(gtweet_object_parent_class)->finalize)
    G_OBJECT_CLASS(gtweet_object_parent_class)->finalize(object);

  g_object_get(object,
	       "consumer_key", &consumer_key,
	       "consumer_secret", &consumer_secret,
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  g_free(consumer_key);
  g_free(consumer_secret);
  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);
}

static void gtweet_object_set_property(GObject *object,
				       guint property_id,
				       const GValue *value,
				       GParamSpec *pspec)
{
  GtweetObject *tweetObject = GTWEET_OBJECT(object);

  g_return_if_fail(GTWEET_IS_OBJECT(object));
  g_return_if_fail(G_VALUE_HOLDS_STRING(value));

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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void gtweet_object_get_property(GObject *object,
				       guint property_id,
				       GValue *value,
				       GParamSpec *pspec)
{
  GtweetObject *tweetObject = GTWEET_OBJECT(object);

  g_return_if_fail(GTWEET_IS_OBJECT(object));
  g_return_if_fail(G_VALUE_HOLDS_STRING(value));

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
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
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
}

GtweetObject* gtweet_object_new(void)
{
  return g_object_new(GTWEET_TYPE_OBJECT, NULL);
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

gchar* gtweet_object_authurl(GtweetObject *tweetObject,
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

static gboolean gtweet_generic_callback(gchar *string, gpointer user_data)
{
  GSimpleAsyncResult *result = (GSimpleAsyncResult *) user_data;
  GCancellable *cancel = g_simple_async_result_get_source_tag(result);

  g_simple_async_result_set_op_res_gpointer(result, string, NULL);
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
  g_free(access_key);
  g_free(access_secret);
  g_object_unref(result);
}
