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

#ifndef __TWEETPTS_TWITTERAPI__
#define __TWEETPTS_TWITTERAPI__

#define T_S_STAT_FILTER "https://stream.twitter.com/1.1/statuses/filter.json"
#define T_S_STAT_SAMPLE "https://stream.twitter.com/1.1/statuses/sample.json"
#define T_S_USER "https://userstream.twitter.com/1.1/user.json"
#define T_R_HTIMELINE "https://api.twitter.com/1.1/statuses/home_timeline.json"
#define T_R_UTIMELINE "https://api.twitter.com/1.1/statuses/user_timeline.json"
#define T_R_ACCOUNTSETTINGS "https://api.twitter.com/1.1/account/settings.json"
#define T_R_TRENDS "https://api.twitter.com/1.1/trends/place.json?id=%s"
#define T_R_TWEETSEARCH "https://api.twitter.com/1.1/search/tweets.json"
#define T_R_LOOKUP "https://api.twitter.com/1.1/users/lookup.json"
#define T_R_USERSEARCH "https://api.twitter.com/1.1/users/search.json"
#define T_R_FOLLOWING "https://api.twitter.com/1.1/friends/ids.json"
#define T_R_FOLLOWERS "https://api.twitter.com/1.1/followers/ids.json"
#define T_R_UPDATEMEDIA "https://api.twitter.com/1.1/statuses/update_with_media.json"
#define T_R_UPDATE "https://api.twitter.com/1.1/statuses/update.json"
#define T_R_RETWEET "https://api.twitter.com/1.1/statuses/retweet/%s.json"
#define T_R_DESTROY "https://api.twitter.com/1.1/statuses/destroy/%s.json"
#define T_R_FOLLOW "https://api.twitter.com/1.1/friendships/create.json"
#define T_R_UNFOLLOW "https://api.twitter.com/1.1/friendships/destroy.json"
#define T_R_BLOCKLIST "https://api.twitter.com/1.1/blocks/list.json"
#define T_R_BLOCK "https://api.twitter.com/1.1/blocks/create.json"
#define T_R_UNBLOCK "https://api.twitter.com/1.1/blocks/destroy.json"
#define T_R_PROFILE "https://api.twitter.com/1.1/account/update_profile.json"
#define T_R_PBACKGROUND "https://api.twitter.com/1.1/account/update_profile_background_image.json"
#define T_R_PIMAGE "https://api.twitter.com/1.1/account/update_profile_image.json"
#define Y_R_WOEID "http://where.yahooapis.com/v1"

void twitterapi_init(void);
void twitterapi_free(void);
void twitterapi_s_stat_filter(gchar *track, gchar *follow, gchar *locations,
			      gpointer func, gpointer userdata);
void twitterapi_s_stat_sample(gpointer func, gpointer userdata);
void twitterapi_s_htimeline(gchar *track, gchar *locations, gpointer func,
			    gpointer userdata);
gchar* twitterapi_r_htimeline(gchar *count, gchar *since_id, gchar *max_id);
gchar* twitterapi_r_utimeline(gchar *userid, gchar *screenname, gchar *since_id,
			      gchar *count, gchar *max_id);
gchar* twitterapi_r_usersettings(void);
gchar* twitterapi_r_woeid(gchar *countryname);
gchar* twitterapi_r_trends(gchar *woeid);
gchar* twitterapi_r_tweetsearch(gchar *q, gchar *geocode, gchar *lang,
				gchar *locale, gchar *result_type, gchar *count,
				gchar *until, gchar *since_id, gchar *max_id);
gchar* twitterapi_r_lookup(gchar *screenname, gchar *user_id);
gchar* twitterapi_r_usersearch(gchar *q, gchar *page, gchar *count);
gchar* twitterapi_r_following(gchar *userid, gchar *screenname, gchar *cursor);
gchar* twitterapi_r_followers(gchar *userid, gchar *screenname, gchar *cursor);
gchar* twitterapi_r_updatemedia(gchar *status, gchar *filepath);
gchar* twitterapi_r_update(gchar *status, gchar *replypostid);
gchar* twitterapi_r_retweet(gchar *postid);
gchar* twitterapi_r_destroy(gchar *postid);
gchar* twitterapi_r_follow(gchar *screenname, gchar *userid);
gchar* twitterapi_r_unfollow(gchar *screenname, gchar *userid);
gchar* twitterapi_r_blocklist(gchar *cursor);
gchar* twitterapi_r_block(gchar *screenname, gchar *userid);
gchar* twitterapi_r_unblock(gchar *screenname, gchar *userid);
gchar* twitterapi_r_profile(gchar *name, gchar *url, gchar *location,
			    gchar *description);
gchar* twitterapi_r_pbackground(gchar *filepath, gchar *tile, gchar *use);
gchar* twitterapi_r_pimage(gchar *filepath);

#endif
