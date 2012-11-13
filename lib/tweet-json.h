#ifndef __TWEET_JSON_H__
#define __TWEET_JSON_H__

#include <json-glib/json-glib.h>

JsonParser* tweet_json_parser(void);
gchar* tweet_json_get_value(JsonNode *root,
			    gchar *field);
JsonNode* tweet_json_get_object(JsonNode *root,
				gchar *field);
JsonNode* tweet_json_decode(JsonParser *parser,
			    gchar *string);
gchar* tweet_json_get_array(JsonNode *root,
			    gchar *fields);

#endif /* __TWEET_JSON_H__ */
