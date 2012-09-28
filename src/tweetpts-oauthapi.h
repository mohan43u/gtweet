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

#ifndef __TWEETPTS_OAUTHAPI__
#define __TWEETPTS_OAUTHAPI__

#include <oauth.h>
#include <wordexp.h>

#define CONSUMER_KEY "EpQdWugEGn0L8sSeDyNxQ"
#define CONSUMER_SECRET "e21wC3jBLlpDtjf4pKrD0m3br7LgPjwMFukhq31Vg"
#define YAHOO_APPID "5jjpbPLV34HGmMDspnGKm0CfyR2QDaEaHulep.T59NmM4eA8rdFRPhCSWvVgd8w-"
#define REQ_TOKEN_URL "https://api.twitter.com/oauth/request_token"
#define AUTH_URL "https://api.twitter.com/oauth/authorize"
#define ACCESS_TOKEN_URL "https://api.twitter.com/oauth/access_token"

#define CMDLINE_FMT "xdg-open '%s&%s&%s'"
#define OAUTHFILE glibapi_expandfilename("~/.tweetpts")

gchar *request_key_pair;
gchar *request_secret_pair;
gchar *request_key;
gchar *request_secret;
gchar *access_key_pair;
gchar *access_secret_pair;
gchar *access_key;
gchar *access_secret;
gchar *pin;

gchar* oauthapi_sign(gchar *url, gchar **params, gchar *method);
void oauthapi_request_token(void);
void oauthapi_authenticate(void);
void oauthapi_access_token(void);
void oauthapi_init(void);
void oauthapi_access_token_from_file(void);
void oauthapi_access_token_to_file(void);
void oauthapi_free(void);

#endif
