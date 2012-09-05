#include <tweetpts.h>

static gchar* jsonapi_stringify(JsonNode *node, gchar *type)
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
	      gboolean value = json_array_get_boolean_element(nodearray, iter);
	      g_string_append_printf(string, "%d", value);
	      if(format)
		g_string_append_printf(string, format, value);
	    }
	  else if(typev && typev[0] && g_strcmp0("double", typev[0]) == 0)
	    {
	      gdouble value = json_array_get_double_element(nodearray, iter);
	      g_string_append_printf(string, "%lf", value);
	      if(format)
		g_string_append_printf(string, format, value);
	    }
	  else if(typev && typev[0] && g_strcmp0("int", typev[0]) == 0)
	    {
	      gint64 value = json_array_get_int_element(nodearray, iter);
	      g_string_append_printf(string, "%ld", value);
	      if(format)
		g_string_append_printf(string, format, value);
	    }
	  else if(typev && typev[0] && g_str_has_prefix(typev[0], "time") == TRUE)
	    {
	      const gchar *value = json_array_get_string_element(nodearray, iter);
	      struct tm utctime;
	      time_t time;
	      gchar *localtime = NULL;
	      memset(&utctime, 0, sizeof(struct tm));
	      if(g_strcmp0("timez", typev[0]) == 0)
		strptime(value, "%FT%TZ", &utctime);
	      else
		strptime(value, "%a %b %d %T %z %Y", &utctime);
	      time = timegm(&utctime);
	      localtime = ctime(&time);
	      g_string_append_printf(string, "%s", g_strstrip(localtime));
	      if(format)
		g_string_append_printf(string, format, localtime);
	    }
	  else
	    {
	      const gchar *value = json_array_get_string_element(nodearray, iter);
	      g_string_append_printf(string, "%s", value);
	      if(format)
		g_string_append_printf(string, format, value);
	    }
	  if((iter + 1) != length)
	    g_string_append(string, " | ");
	}
      g_free(format);

      result = g_strdup(string->str);
      g_strfreev(typev);
      g_string_free(string, TRUE);
    }
  return(result);
}

gchar* jsonapi_get_value(JsonNode *root, gchar *field)
{
  gchar *string = NULL;
  JsonNode *result = NULL;

  if(root && field)
    {
      gchar *jsonpath = NULL;
      gchar **fieldv = g_strsplit(field, "|", 2);
      jsonpath = g_strdup_printf("$%s", fieldv[0]);
      result = json_path_query(jsonpath, root, NULL);
      g_free(jsonpath);
      string = jsonapi_stringify(result, fieldv[1]);
      json_node_free(result);
      g_strfreev(fieldv);
    }
  return(string);
}

JsonNode* jsonapi_decode(JsonParser *parser, gchar *string)
{
  JsonNode *root = NULL;

  json_parser_load_from_data(parser, string, -1, NULL);
  root = json_parser_get_root(parser);
  return(root);
}

JsonParser* jsonapi_parser(void)
{
  return(json_parser_new());
}

guint jsonapi_length(JsonNode *root)
{
  if(JSON_NODE_HOLDS_ARRAY(root))
    {
      JsonArray *array = json_node_get_array(root);
      return(json_array_get_length(array));
    }
  else
    return(0);
}

JsonNode* jsonapi_get_element(JsonNode *root, guint index)
{
  if(JSON_NODE_HOLDS_ARRAY(root))
    {
      JsonArray *array = json_node_get_array(root);
      return(json_array_get_element(array, index));
    }
  else
    return(NULL);
}

void jsonapi_init(void)
{
  g_type_init();
}

void jsonapi_free(void)
{
}
