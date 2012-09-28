/* 
 * Copyright (c) <2012>, Mohan R <mohan43u@gmail.com>
 * All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 * 
 *   1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 *   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those
 *   of the authors and should not be interpreted as representing official policies,
 *   either expressed or implied, of the FreeBSD Project.
 */

#ifndef __TWEETPTS_JSONAPI__
#define __TWEETPTS_JSONAPI__

#include <json-glib/json-glib.h>

void jsonapi_init(void);
gchar* jsonapi_get_value(JsonNode *root, gchar *field);
JsonNode* jsonapi_get_object(JsonNode *root, gchar *field);
JsonNode* jsonapi_decode(JsonParser *parser, gchar *string);
JsonParser* jsonapi_parser(void);
guint jsonapi_length(JsonNode *root);
JsonNode* jsonapi_get_element(JsonNode *root, guint index);
gchar* jsonapi_get_array(JsonNode *root, gchar *fields);
void jsonapi_free(void);

#define J_FIELD "text|$..text,"					\
  "fname|$.user.name,"						\
  "tname|$.user.screen_name|| [http://www.twitter.com/%s],"	\
  "official|$.user.verified|boolean,"				\
  "time|$.created_at|time,"					\
  "postid|$.id_str,"						\
  "url|$..url,"							\
  "desc|$.user.description,"					\
  "loc|$.user.location,"					\
  "source|$.source"

#define J_FILTER_FIELD J_FIELD
#define J_SAMPLE_FIELD J_FIELD
#define J_FIREHOSE_FIELD J_FIELD
#define J_HTIMELINE_FIELD J_FIELD
#define J_UTIMELINE_FIELD J_FIELD

#define J_USERSETTINGS_FIELD "name|$.screen_name,"	\
  "country|$.trend_location[*].country,"		\
  "place|$.time_zone.name,"				\
  "following|$.following|int"				\
  "tz|$.time_zone.tzinfo_name,"				\
  "utc_offset|$.time_zone.utc_offset|int"

#define J_TRENDS_FIELD "location|$..locations..name,"	\
  "woeid|$..locations..woeid|int,"			\
  "as_of|$..as_of|timez,"				\
  "trends|$..trends..name"

#define J_TWEETSEARCH_FIELD "$.statuses.*|"J_FIELD

#define J_LOOKUP_FIELD "fname|$.name,"			\
  "tname|$.screen_name|| [http://www.twitter.com/%s],"	\
  "official|$.verified|boolean,"			\
  "desc|$.description,"					\
  "location|$.location,"				\
  "id|$.id_str,"					\
  "url|$..url,"						\
  "tz|$.time_zone,"					\
  "created_at|$.created_at|time,"			\
  "nfollowing|$.friends_count|int,"			\
  "nfollowers|$.followers_count|int,"			\
  "ntweets|$.statuses_count|int,"			\
  "image|$.profile_image_url_https,"			\
  "background|$.profile_background_image_url_https,"	\
  "text|$.status.text,"					\
  "time|$.status.created_at|time,"			\
  "postid|$.status.id_str,"				\
  "source|$.status.source"

#define J_USERSEARCH_FIELD J_LOOKUP_FIELD
#define J_FOLLOWING_FIELD J_LOOKUP_FIELD
#define J_FOLLOWERS_FIELD J_LOOKUP_FIELD
#define J_UPDATEMEDIA_FIELD J_FIELD
#define J_UPDATE_FIELD J_FIELD
#define J_RETWEET_FIELD J_FIELD
#define J_DESTROY_FIELD J_FIELD
#define J_FOLLOW_FIELD J_LOOKUP_FIELD
#define J_UNFOLLOW_FIELD J_LOOKUP_FIELD
#define J_BLOCKLIST_FIELD J_LOOKUP_FIELD
#define J_BLOCK_FIELD J_LOOKUP_FIELD
#define J_UNBLOCK_FIELD J_LOOKUP_FIELD
#define J_PROFILE_FIELD J_LOOKUP_FIELD
#define J_PBACKGROUND_FIELD J_LOOKUP_FIELD
#define J_PIMAGE_FIELD J_LOOKUP_FIELD

#endif
