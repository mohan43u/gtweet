#include <tweetpts.h>

static void jsonwalk(JsonNode *root, GString *output);

static void arraycallback(JsonArray *array,
		   guint index_,
		   JsonNode *element_node,
		   gpointer user_data)
{
  jsonwalk(element_node, (GString *) user_data);
}

static void objectcallback(JsonObject *object,
		    const gchar *member_name,
		    JsonNode *member_node,
		    gpointer user_data)
{
  GString *output = (GString *) user_data;
  g_string_append_printf(output, "%s: ", member_name);
  jsonwalk(member_node, output);
}

static void parsevalue(JsonNode *node, GString *output)
{
  GType type = json_node_get_value_type(node);
  switch(type)
    {
    case G_TYPE_BOOLEAN:
      {
	g_string_append_printf(output, "%d", json_node_get_boolean(node));
      }
      break;
    case G_TYPE_DOUBLE:
      {
	g_string_append_printf(output, "%lf", json_node_get_double(node));
      }
      break;
    case G_TYPE_INT:
      {
	g_string_append_printf(output, "%d", json_node_get_int(node));
      }
      break;
    case G_TYPE_STRING:
      {
	g_string_append_printf(output, "%s", json_node_get_string(node));
      }
    }
  g_string_append_c(output, '\n');
}

static void jsonwalk(JsonNode *root, GString *output)
{
  switch(JSON_NODE_TYPE(root))
    {
    case JSON_NODE_ARRAY:
      {
	JsonArray *array = json_node_get_array(root);
	g_string_append_c(output, '\n');
	json_array_foreach_element(array,
				   arraycallback,
				   (gpointer) output);
      }
      break;
    case JSON_NODE_OBJECT:
      {
	JsonObject *object = json_node_get_object(root);
	g_string_append_c(output, '\n');
	json_object_foreach_member(object,
				   objectcallback,
				   (gpointer) output);
      }
      break;
    case JSON_NODE_VALUE:
      {
	parsevalue(root, output);
      }
      break;
    default:
      g_string_append_c(output, '\n');
    }
}

void parsejson(GString *content, GString *output)
{
  JsonParser *parser = NULL;
  JsonNode *root = NULL;
  GError *error = NULL;

  g_type_init();

  parser = json_parser_new();
  if(json_parser_load_from_data(parser,
				content->str,
				content->len,
				&error) != TRUE)
    {
      g_print("%s: %s\n", g_quark_to_string(error->domain), error->message);
      exit(1);
    }
  root = json_parser_get_root(parser);
  jsonwalk(root, output);

  g_object_unref(parser);
}
