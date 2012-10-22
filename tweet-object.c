#include <tweet.h>

static void g_tweet_object_class_init(GTweetObjectClass *klass);
static void g_tweet_object_init(GTweetObject *object);
G_DEFINE_TYPE(GTweetObject, g_tweet_object, G_TYPE_OBJECT);

static GObject* g_tweet_object_constructor(GType type,
					   guint n_construct_properties,
					   GObjectConstructParam *construct_properties)
{
  GObject *object = NULL;

  object = G_OBJECT_CLASS(g_tweet_object_parent_class)->constructor(type,
								    n_construct_properties,
								    construct_properties);
  return(object);
}

static void g_tweet_object_finalize(GObject *object)
{
  GTweetObject *obj = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  obj = G_TWEETOBJECT(object);
  g_return_if_fail(G_IS_TWEETOBJECT(object));
  if(G_OBJECT_CLASS(g_tweet_object_parent_class)->finalize)
    G_OBJECT_CLASS(g_tweet_object_parent_class)->finalize(object);

  g_object_get(object,
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);

  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);
}

static void g_tweet_object_set_property(GObject *object,
					guint property_id,
					const GValue *value,
					GParamSpec *pspec)
{
  GTweetObject *tweetObject = G_TWEETOBJECT(object);

  g_return_if_fail(G_IS_TWEETOBJECT(object));
  g_return_if_fail(G_VALUE_HOLDS_STRING(value));

  switch(property_id)
    {
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

static void g_tweet_object_get_property(GObject *object,
					guint property_id,
					GValue *value,
					GParamSpec *pspec)
{
  GTweetObject *tweetObject = G_TWEETOBJECT(object);

  g_return_if_fail(G_IS_TWEETOBJECT(object));
  g_return_if_fail(G_VALUE_HOLDS_STRING(value));

  switch(property_id)
    {
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

static void g_tweet_object_class_init(GTweetObjectClass *klass)
{
  GObjectClass *gobjectClass = G_OBJECT_CLASS(klass);
  gobjectClass->constructor = g_tweet_object_constructor;
  gobjectClass->finalize = g_tweet_object_finalize;
  gobjectClass->set_property = g_tweet_object_set_property;
  gobjectClass->get_property = g_tweet_object_get_property;

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

static void g_tweet_object_init(GTweetObject *self)
{
  g_object_ref(self);
}

GTweetObject* g_tweet_object_new(void)
{
  return(g_object_new(G_TYPE_TWEETOBJECT, NULL));
}

gboolean g_tweet_object_initkeys(GTweetObject *tweetObject)
{
  gchar *access_key = NULL;
  gchar *access_secret = NULL;
  gboolean result = FALSE;

  result = tweet_oauth_access_token_from_file(&access_key,
					      &access_secret);
  if(result == TRUE)
    g_object_set(tweetObject,
		 "access_key", access_key,
		 "access_secret", access_secret,
		 NULL);

  g_free(access_key);
  g_free(access_secret);

  return(result);
}

gchar* g_tweet_object_authurl(GTweetObject *tweetObject)
{
  gchar *authurl = NULL;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;

  tweet_oauth_request_token(&request_key,
			    &request_secret);
  if(request_key && request_secret)
    {
      authurl = tweet_oauth_gen_authurl(request_key,
					request_secret);
      g_object_set(G_OBJECT(tweetObject),
		   "request_key", request_key,
		   "request_secret", request_secret,
		   NULL);
    }

  g_free(request_key);
  g_free(request_secret);

  return(authurl);
}

gboolean g_tweet_object_auth(GTweetObject *tweetObject,
			     gchar *pin)
{
  gboolean result = FALSE;
  gchar *request_key = NULL;
  gchar *request_secret = NULL;
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "request_key", &request_key,
	       "request_secret", &request_secret,
	       NULL);

  tweet_oauth_access_token(pin,
			   request_key,
			   request_secret,
			   &access_key,
			   &access_secret);

  if(access_key && access_secret)
    {
      result = TRUE;
      tweet_oauth_access_token_to_file(access_key,
				       access_secret);
      g_object_set(G_OBJECT(tweetObject),
		   "access_key", access_key,
		   "access_secret", access_secret,
		   NULL);
    }

  g_free(request_key);
  g_free(request_secret);
  g_free(access_key);
  g_free(access_secret);

  return(result);
}

void g_tweet_object_samplestream(GTweetObject *tweetObject,
				 GTweetObjectStreamFunc func,
				 gpointer userdata,
				 GDestroyNotify notify)
{
  gchar *access_key = NULL;
  gchar *access_secret = NULL;

  g_object_get(G_OBJECT(tweetObject),
	       "access_key", &access_key,
	       "access_secret", &access_secret,
	       NULL);
  tweet_twitter_s_stat_sample(access_key,
			      access_secret,
			      func,
			      userdata);

  g_free(access_key);
  g_free(access_secret);
}
