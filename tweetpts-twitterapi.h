#ifndef __TWEETPTS_TWITTERAPI__
#define __TWEETPTS_TWITTERAPI__

#define T_S_STAT_FILTER "https://stream.twitter.com/1.1/statuses/filter.json"
#define T_S_STAT_SAMPLE "https://stream.twitter.com/1.1/statuses/sample.json"
#define T_S_STAT_FIREHOSE "https://stream.twitter.com/1.1/statuses/firehose.json"
#define T_S_USER "https://userstream.twitter.com/2/user.json"
#define T_R_TIMELINE "https://api.twitter.com/1.1/statuses/home_timeline.json"
#define T_R_ACCOUNTSETTINGS "https://api.twitter.com/1.1/account/settings.json"
#define T_R_TRENDS "https://api.twitter.com/1.1/trends/place.json?id=%s"
#define Y_R_WOEID "http://where.yahooapis.com/v1"

void twitterapi_init(void);
void twitterapi_free(void);
void twitterapi_set_userfields(gchar *fields);
gchar* twitterapi_get_userfields(void);
void twitterapi_s_stat_filter(GSList *args);
void twitterapi_s_stat_sample(GSList *args);
void twitterapi_s_stat_firehose(GSList *args);
void twitterapi_s_timeline(GSList *args);
gchar* twitterapi_r_timeline(gchar *count);
gchar* twitterapi_get_usersettings(void);
gchar* twitterapi_get_trends(gchar *woeid);
gchar* twitterapi_get_woeid(gchar *countryname);


#define T_FIELD "text|..text,"					\
  "screenname|.user.screen_name|| [http://www.twitter.com/%s]," \
  "time|.created_at|time,"					\
  "postid|.id_str,"						\
  "userid|.user.id_str,"					\
  "url|..url,"							\
  "desc|.user.description,"					\
  "location|.user.location,"					\
  "source|.source,"						\
  "image|.user.profile_image_url_https,"			\
  "background|.user.profile_background_image_url_https"

#define T_FILTER_FIELD T_FIELD
#define T_SAMPLE_FIELD T_FIELD
#define T_FIREHOSE_FIELD T_FIELD
#define T_TIMELINE_FIELD T_FIELD

#define T_USERSETTINGS_FIELD "name|.screen_name,"	\
  "country|.trend_location[*].country,"			\
  "place|.time_zone.name,"				\
  "following|.following|int"				\
  "nfollowers|.followers_count|int,"			\
  "nfriends|.friends_count|int,"			\
  "tz|.time_zone.tzinfo_name,"				\
  "utc_offset|.time_zone.utc_offset|int"

#define T_TRENDS_FIELD "location|..locations..name,"	\
  "woeid|..locations..woeid|int,"			\
  "trends|..trends..name,"				\
  "as_of|..as_of|timez"

#define TRENDUPDATESEC (5*60)

typedef struct twitterapi_field
{
  GString *fields;
  GMutex mutex;
} XFIELD;

XFIELD *twitterapi_userfields;
XFIELD *twitterapi_woeid;

#endif
