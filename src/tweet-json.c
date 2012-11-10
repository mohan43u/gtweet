#include <tweet.h>

static guint tweet_json_length(JsonNode *root)
{
  guint returnvalue = 0;
  if(JSON_NODE_HOLDS_ARRAY(root))
    {
      JsonArray *array = json_node_get_array(root);
      returnvalue = json_array_get_length(array);
    }
  else if(json_node_is_null(root) != TRUE)
    returnvalue = 1;

  return returnvalue;
}

static gchar* tweet_json_stringify(JsonNode *node,
				   gchar *type)
{
  gchar *result = NULL;

  if(JSON_NODE_HOLDS_ARRAY(node))
    {
      JsonArray *nodearray = NULL;
      guint length = 0;
      guint iter = 0;
      GString *string = g_string_new(NULL);
      gchar **typev = NULL;
      guint argc = 0;
      gchar *format = NULL;

      nodearray = json_node_get_array(node);
      length = json_array_get_length(nodearray);

      if(type)
	{
	  typev = g_strsplit(type, "|", 0);
	  while(typev[argc]) argc++;
	  if(argc >= 2)
	    format = g_strdup_printf("%s", typev[1]);
	}

      for(iter = 0; iter < length; iter++)
	{
	  if(typev && typev[0] && g_strcmp0("boolean", typev[0]) == 0)
	    {
	      gboolean value = 0;
	      value = json_array_get_boolean_element(nodearray, iter);
	      if(value)
		{
		  g_string_append_printf(string, "%d", value);
		  if(format)
		    g_string_append_printf(string, format, value);
		  if((iter + 1) != length)
		    g_string_append(string, " ~ ");
		}
	    }
	  else if(typev && typev[0] && g_strcmp0("double", typev[0]) == 0)
	    {
	      gdouble value = 0;
	      value = json_array_get_double_element(nodearray, iter);
	      if(value)
		{
		  g_string_append_printf(string, "%lf", value);
		  if(format)
		    g_string_append_printf(string, format, value);
		  if((iter + 1) != length)
		    g_string_append(string, " ~ ");
		}
	    }
	  else if(typev && typev[0] && g_strcmp0("int", typev[0]) == 0)
	    {
	      gint64 value = 0;
	      value = json_array_get_int_element(nodearray, iter);
	      if(value)
		{
		  g_string_append_printf(string, "%ld", value);
		  if(format)
		    g_string_append_printf(string, format, value);
		  if((iter + 1) != length)
		    g_string_append(string, " ~ ");
		}
	    }
	  else if(typev && typev[0] &&
		  g_str_has_prefix(typev[0], "time") == TRUE)
	    {
	      const gchar *value = NULL;
	      struct tm utctime;
	      time_t time;
	      gchar *localtime = NULL;

	      value = json_array_get_string_element(nodearray, iter);
	      memset(&utctime, 0, sizeof(struct tm));
	      if(g_strcmp0("timez", typev[0]) == 0)
		strptime(value, "%FT%TZ", &utctime);
	      else
		strptime(value, "%a %b %d %T %z %Y", &utctime);
	      time = timegm(&utctime);
	      localtime = ctime(&time);
	      if(localtime && strlen(localtime))
		{
		  g_string_append_printf(string, "%s", g_strstrip(localtime));
		  if(format)
		    g_string_append_printf(string, format, localtime);
		  if((iter + 1) != length)
		    g_string_append(string, " ~ ");
		}
	    }
	  else
	    {
	      JsonNode *node = json_array_get_element(nodearray, iter);
	      if(JSON_NODE_HOLDS_VALUE(node))
		{
		  const gchar *value = NULL;
		  value = json_node_get_string(node);
		  if(value && strlen(value))
		    {
		      g_string_append_printf(string, "%s", value);
		      if(format)
			g_string_append_printf(string, format, value);
		      if((iter + 1) != length)
			g_string_append(string, " ~ ");
		    }
		}
	    }
	}
      g_free(format);

      result = g_strdup(string->str);
      g_strfreev(typev);
      g_string_free(string, TRUE);
    }
  return result;
}

static JsonNode* tweet_json_get_element(JsonNode *root, guint index)
{
  JsonNode *returnNode = NULL;

  if(JSON_NODE_HOLDS_ARRAY(root))
    {
      JsonArray *array = json_node_get_array(root);
      returnNode = json_array_get_element(array, index);
    }
  if(JSON_NODE_HOLDS_OBJECT(root))
    returnNode = root;

  return returnNode;
}

JsonParser* tweet_json_parser(void)
{
  g_type_init();
  return json_parser_new();
}

gchar* tweet_json_get_value(JsonNode *root,
			    gchar *field)
{
  gchar *string = NULL;
  JsonNode *result = NULL;

  if(root && field && strlen(field))
    {
      gchar **fieldv = g_strsplit(field, "|", 2);
      result = json_path_query(fieldv[0], root, NULL);
      string = tweet_json_stringify(result, fieldv[1]);
      json_node_free(result);
      g_strfreev(fieldv);
    }
  return string;
}

JsonNode* tweet_json_get_object(JsonNode *root,
				gchar *field)
{
  JsonNode *result = NULL;

  if(root && field)
    {
      gchar *jsonpath = NULL;
      result = json_path_query(field, root, NULL);
      if(JSON_NODE_HOLDS_ARRAY(result))
	result = json_array_get_element(json_node_get_array(result), 0);
    }

  return result;
}

JsonNode* tweet_json_decode(JsonParser *parser,
			    gchar *string)
{
  JsonNode *root = NULL;

  json_parser_load_from_data(parser, string, -1, NULL);
  root = json_parser_get_root(parser);
  return root;
}

gchar* tweet_json_get_array(JsonNode *root,
			    gchar *fields)
{
  gchar *result = NULL;
  GString *string = NULL;
  if(JSON_NODE_HOLDS_ARRAY(root))
    {
      guint iter = 0;
      guint length = tweet_json_length(root);

      string = g_string_new(NULL);
      while(iter < length)
	{
	  JsonNode *node = tweet_json_get_element(root, iter);
	  gchar *value = tweet_json_get_value(node, fields);
	  g_string_append_printf(string, "%s,", value);
	  g_free(value);
	  iter++;
	}
      g_string_truncate(string, string->len - 1);
      result = g_strdup(string->str);
      g_string_free(string, TRUE);
    }

  return result;
}
