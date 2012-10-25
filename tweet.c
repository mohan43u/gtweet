#include <stdio.h>
#include <glib-unix.h>
#include <gtk/gtk.h>
#include <tweet.h>

typedef struct
{
  GtkWidget *inputDialog;
  GtkWidget *actionArea;
  GtkWidget *contentArea;
  GtkWidget *actionAreaHbox;
  GtkWidget *contentAreaHbox;
  GtkWidget *actionAreaOkButton;
  GtkWidget *actionAreaCancelButton;
  GtkWidget *contentAreaLabel;
  GtkWidget *contentAreaEntry;
  gchar *text;
} InputWidget;

InputWidget* input_widget_new(const gchar *text)
{
  InputWidget *inputWidget = g_new0(InputWidget, 1);

  /*createing new GtkDialog*/
  inputWidget->inputDialog = gtk_dialog_new();

  /*getting action and content areas*/
  inputWidget->actionArea = gtk_dialog_get_action_area(GTK_DIALOG(inputWidget->inputDialog));
  inputWidget->contentArea = gtk_dialog_get_content_area(GTK_DIALOG(inputWidget->inputDialog));

  /*creating action area box*/
  inputWidget->actionAreaHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_set_homogeneous(GTK_BOX(inputWidget->actionAreaHbox), TRUE);
  gtk_box_set_spacing(GTK_BOX(inputWidget->actionAreaHbox), 2);

  /*adding ok and cancel button into action area box*/
  inputWidget->actionAreaOkButton = gtk_button_new_from_stock(GTK_STOCK_OK);
  inputWidget->actionAreaCancelButton = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_end(GTK_BOX(inputWidget->actionAreaHbox),
		   inputWidget->actionAreaOkButton, TRUE, TRUE, 2);
  gtk_box_pack_end(GTK_BOX(inputWidget->actionAreaHbox),
		   inputWidget->actionAreaCancelButton, TRUE, TRUE, 2);

  /*creating content area box*/
  inputWidget->contentAreaHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_set_homogeneous(GTK_BOX(inputWidget->contentAreaHbox), FALSE);
  gtk_box_set_spacing(GTK_BOX(inputWidget->contentAreaHbox), 2);

  /*adding label and entry to content area*/
  inputWidget->contentAreaLabel = gtk_label_new(text);
  inputWidget->contentAreaEntry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(inputWidget->contentAreaHbox),
		     inputWidget->contentAreaLabel, FALSE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(inputWidget->contentAreaHbox),
		     inputWidget->contentAreaEntry, FALSE, FALSE, 2);

  /*adding action area box and content area box to respection areas*/
  gtk_box_pack_start(GTK_BOX(inputWidget->actionArea),
		     inputWidget->actionAreaHbox, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(inputWidget->contentArea),
		     inputWidget->contentAreaHbox, TRUE, TRUE, 0);

  return(inputWidget);
}

void ok_button_cb(GtkButton *button, gpointer user_data)
{
  InputWidget *inputWidget = (InputWidget *) user_data;
  inputWidget->text = g_strdup(gtk_entry_get_text(GTK_ENTRY(inputWidget->contentAreaEntry)));
  gtk_widget_destroy(inputWidget->inputDialog);
  gtk_main_quit();
}

void cancel_button_cb(GtkButton *button, gpointer user_data)
{
  InputWidget *inputWidget = (InputWidget *) user_data;
  gtk_widget_destroy(inputWidget->inputDialog);
  gtk_main_quit();
}

void dialog_delete_event_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  gtk_widget_destroy(widget);
  gtk_main_quit();
}

gchar* input_widget_get_text(int argc, char *argv[])
{
  InputWidget *inputWidget = NULL;
  gchar *text = NULL;

  gtk_init(&argc, &argv);

  inputWidget = input_widget_new("PIN");
  g_object_connect(G_OBJECT(inputWidget->actionAreaOkButton),
		   "signal::clicked", ok_button_cb, inputWidget,
		   NULL);
  g_object_connect(G_OBJECT(inputWidget->actionAreaCancelButton),
		   "signal::clicked", cancel_button_cb, inputWidget,
		   NULL);
  g_object_connect(G_OBJECT(inputWidget->inputDialog),
		   "signal::delete-event", dialog_delete_event_cb, NULL,
		   NULL);

  gtk_widget_show_all(inputWidget->inputDialog);
  gtk_main();

  text = inputWidget->text;
  g_free(inputWidget);
  return(text);
}

static gboolean sample_stream_cb(gchar *string,
				 gpointer userdata)
{
  g_print("%s\n", string);
}

int main(int argc, char *argv[])
{
  GtweetObject *tweetObject = NULL;

  g_type_init();

  tweetObject = GTWEET_OBJECT(gtweet_object_new());
  if(!gtweet_object_initkeys(tweetObject))
    {
      gchar *authurl = NULL;
      gchar *cmdline = NULL;
      gchar *pin = NULL;

      authurl = gtweet_object_authurl(tweetObject);
      cmdline = g_strdup_printf("xdg-open '%s'", authurl);
      if(g_spawn_command_line_sync(cmdline, NULL, NULL, NULL, NULL))
	{
	  pin = input_widget_get_text(argc, argv);
	  gtweet_object_auth(tweetObject, pin);
	}

      g_free(authurl);
      g_free(cmdline);
      g_free(pin);
    }

  gtweet_object_samplestream(tweetObject,
			     sample_stream_cb,
			     NULL);

  g_object_unref(tweetObject);

  return(0);
}
