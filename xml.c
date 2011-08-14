#include <tweetpts.h>

void start_element_callback(GMarkupParseContext *context,
			    const gchar *element_name,
			    const gchar **attribute_names,
			    const gchar **attribute_values,
			    gpointer userdata,
			    GError **error)
{
  GString *key = (GString *) userdata;
  g_string_assign(key, element_name);
  g_string_append(key, " : ");
}

void text_callback(GMarkupParseContext *context,
		   const gchar *text,
		   gsize text_len,
		   gpointer userdata,
		   GError **error)
{
  GString *key = (GString *) userdata;
  gchar *newstring = g_strndup(text, text_len);
  g_string_append(key, newstring);
  g_free(newstring);
}

void end_element_callback(GMarkupParseContext *context,
		   const gchar *element_name,
		   gpointer userdata,
		   GError **error)
{
  GString *key = (GString *) userdata;
  g_print("%s\n", key->str);
}  

void printxml(GString *output)
{
  GError *error = NULL;
  GString *key = g_string_new(NULL);
  GMarkupParser parser = {start_element_callback,
			  end_element_callback,
			  text_callback,
			  NULL,
			  NULL};
  GMarkupParseContext *context = g_markup_parse_context_new(&parser,
							    0,
							    (gpointer) key,
							    NULL);

  if(g_markup_parse_context_parse(context,
				  output->str,
				  output->len,
				  &error) != TRUE)
    {
      g_print("%s\n", error->message);
      exit(1);
    }

  g_string_free(key, TRUE);
}
