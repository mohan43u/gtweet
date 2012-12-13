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

#ifndef __TWEET_TWITTER_H__
#define __TWEET_TWITTER_H__

#define T_S_STAT_FILTER "https://stream.twitter.com/1.1/statuses/filter.json"
#define T_S_STAT_SAMPLE "https://stream.twitter.com/1.1/statuses/sample.json"
#define T_S_USER "https://userstream.twitter.com/1.1/user.json"
#define T_R_HTIMELINE "https://api.twitter.com/1.1/statuses/home_timeline.json"
#define T_R_UTIMELINE "https://api.twitter.com/1.1/statuses/user_timeline.json"
#define T_R_ACCOUNTSETTINGS "https://api.twitter.com/1.1/account/settings.json"
#define T_R_TRENDS "https://api.twitter.com/1.1/trends/place.json?id=%s"
#define T_R_TWEETSEARCH "https://api.twitter.com/1.1/search/tweets.json"
#define T_R_LOOKUP "https://api.twitter.com/1.1/users/lookup.json"
#define T_R_SHOW "http://api.twitter.com/1.1/users/show.json"
#define T_R_USERSEARCH "https://api.twitter.com/1.1/users/search.json"
#define T_R_FOLLOWING "https://api.twitter.com/1.1/friends/ids.json"
#define T_R_FOLLOWERS "https://api.twitter.com/1.1/followers/ids.json"
#define T_R_UPDATEMEDIA "https://api.twitter.com/1.1/statuses/update_with_media.json"
#define T_R_UPDATE "https://api.twitter.com/1.1/statuses/update.json"
#define T_R_RETWEET "https://api.twitter.com/1.1/statuses/retweet/%s.json"
#define T_R_SHOWSTATUS "https://api.twitter.com/1.1/statuses/show/%s.json&include_my_retweet=true"
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

void tweet_twitter_s_stat_filter(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *track,
				 gchar *follow,
				 gchar *locations,
				 gpointer func,
				 gpointer userdata);
void tweet_twitter_s_stat_sample(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gpointer func,
				 gpointer userdata);
void tweet_twitter_s_htimeline(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *track,
			       gchar *locations,
			       gpointer func,
			       gpointer userdata);
gchar* tweet_twitter_r_htimeline(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *count,
				 gchar *since_id,
				 gchar *max_id,
				 gchar *trimuser);
gchar* tweet_twitter_r_utimeline(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *since_id,
				 gchar *count,
				 gchar *max_id);
gchar* tweet_twitter_r_usersettings(gchar *consumer_key,
				    gchar *consumer_secret,
				    gchar *access_key,
				    gchar *access_secret);
gchar* tweet_twitter_r_woeid(gchar *countryname);
gchar* tweet_twitter_r_trends(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *woeid);
gchar* tweet_twitter_r_tweetsearch(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *q,
				   gchar *geocode,
				   gchar *lang,
				   gchar *locale,
				   gchar *result_type,
				   gchar *count,
				   gchar *until,
				   gchar *since_id,
				   gchar *max_id);
gchar* tweet_twitter_r_lookup(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *screenname,
			      gchar *user_id);
gchar* tweet_twitter_r_show(gchar *consumer_key,
			    gchar *consumer_secret,
			    gchar *access_key,
			    gchar *access_secret,
			    gchar *user_id,
			    gchar *screenname);
gchar* tweet_twitter_r_usersearch(gchar *consumer_key,
				  gchar *consumer_secret,
				  gchar *access_key,
				  gchar *access_secret,
				  gchar *q,
				  gchar *page,
				  gchar *count);
static gchar* tweet_twitter_strsplit(gchar **str,
				     gchar *delimit,
				     guint length);
gchar* tweet_twitter_r_following(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *cursor);
gchar* tweet_twitter_r_followers(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *userid,
				 gchar *screenname,
				 gchar *cursor);
gchar* tweet_twitter_r_updatemedia(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *status,
				   gchar *filepath,
				   gchar *replypostid);
gchar* tweet_twitter_r_update(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *status,
			      gchar *replypostid);
gchar* tweet_twitter_r_retweet(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *postid);
gchar* tweet_twitter_r_showstatus(gchar *consumer_key,
				  gchar *consumer_secret,
				  gchar *access_key,
				  gchar *access_secret,
				  gchar *postid);
gchar* tweet_twitter_r_destroy(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *postid);
gchar* tweet_twitter_r_follow(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *screenname,
			      gchar *userid);
gchar* tweet_twitter_r_unfollow(gchar *consumer_key,
				gchar *consumer_secret,
				gchar *access_key,
				gchar *access_secret,
				gchar *screenname,
				gchar *userid);
gchar* tweet_twitter_r_blocklist(gchar *consumer_key,
				 gchar *consumer_secret,
				 gchar *access_key,
				 gchar *access_secret,
				 gchar *cursor);
gchar* tweet_twitter_r_block(gchar *consumer_key,
			     gchar *consumer_secret,
			     gchar *access_key,
			     gchar *access_secret,
			     gchar *screenname,
			     gchar *userid);
gchar* tweet_twitter_r_unblock(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *screenname,
			       gchar *userid);
gchar* tweet_twitter_r_profile(gchar *consumer_key,
			       gchar *consumer_secret,
			       gchar *access_key,
			       gchar *access_secret,
			       gchar *name,
			       gchar *purl,
			       gchar *location,
			       gchar *description);
gchar* tweet_twitter_r_pbackground(gchar *consumer_key,
				   gchar *consumer_secret,
				   gchar *access_key,
				   gchar *access_secret,
				   gchar *filepath,
				   gchar *tile,
				   gchar *use);
gchar* tweet_twitter_r_pimage(gchar *consumer_key,
			      gchar *consumer_secret,
			      gchar *access_key,
			      gchar *access_secret,
			      gchar *filepath);

#endif /* __TWEET_TWITTER_H__ */
