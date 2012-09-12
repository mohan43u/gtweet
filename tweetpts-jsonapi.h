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

#endif
