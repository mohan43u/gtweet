#include <tweetpts.h>

static gchar* jsonapi_stringify(JsonNode *node, gchar *type)
{
  gchar *result = NULL;

  if(JSON_NODE_HOLDS_ARRAY(node))
    {
      JsonArray *nodearray = NULL;
      guint length = 0;
      guint iter = 0;
      gchar *string = NULL;
      GString *buffer = g_string_new(NULL);
      gchar **typev = NULL;
      gchar delimiter = '|';

      nodearray = json_node_get_array(node);
      length = json_array_get_length(nodearray);

      if(type)
	{
	  typev = g_strsplit(type, "|", 2);
	  if(typev[1] && g_strcmp0("newline", typev[1]) == 0)
	    delimiter = '\n';
	}

      for(iter = 0; iter < length; iter++)
	{
	  if(typev && typev[0] && g_strcmp0("boolean", typev[0]) == 0)
	    {
	      gboolean value = json_array_get_boolean_element(nodearray, iter);
	      string = g_strdup_printf("%d", value);
	    }
	  else if(typev && typev[0] && g_strcmp0("double", typev[0]) == 0)
	    {
	      gdouble value = json_array_get_double_element(nodearray, iter);
	      string = g_strdup_printf("%lf", value);
	    }
	  else if(typev && typev[0] && g_strcmp0("int", typev[0]) == 0)
	    {
	      gint64 value = json_array_get_int_element(nodearray, iter);
	      string = g_strdup_printf("%ld", value);
	    }
	  else
	    {
	      const gchar *value = json_array_get_string_element(nodearray, iter);
	      string = g_strdup(value);
	    }
	  if(delimiter == '\n')
	    {
	      g_string_append_printf(buffer, "\n%s", string);
	      g_free(string);
	    }
	  else
	    {
	      if((iter + 1) == length)
		g_string_append_printf(buffer, " %s", string);
	      else
		g_string_append_printf(buffer, " %s %c", string, delimiter);
	      g_free(string);
	    }
	}

      result = g_strdup(buffer->str);
      g_strfreev(typev);
      g_string_free(buffer, TRUE);
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
