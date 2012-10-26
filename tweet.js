#!/usr/bin/env gjs
// after compilation, execute this program with
// LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$(pwd)" GI_TYPELIB_PATH="${GI_TYPELIB_PATH}:$(pwd)" ./tweet.js

var InputWidget = function(text) {
    this.inputDialog = new Gtk.Dialog();
    this.inputDialog.set_title("tweet");

    this.actionArea = this.inputDialog.get_action_area();
    this.contentArea = this.inputDialog.get_content_area();

    this.actionAreaHbox = new Gtk.Box({orientation:Gtk.Orientation.HORIZONTAL, spacing:2});
    this.actionAreaHbox.set_homogeneous(true);
    this.actionAreaOkButton = Gtk.Button.new_from_stock("OK");
    this.actionAreaCancelButton = Gtk.Button.new_from_stock("CANCEL");
    this.actionAreaHbox.pack_end(this.actionAreaOkButton, true, true, 2);
    this.actionAreaHbox.pack_end(this.actionAreaCancelButton, true, true, 2);

    this.contentAreaHbox = new Gtk.Box({orientation:Gtk.Orientation.HORIZONTAL, spacing:2});
    this.contentAreaHbox.set_homogeneous(false);
    this.contentAreaLabel = new Gtk.Label({label:text});
    this.contentAreaEntry = new Gtk.Entry();
    this.contentAreaHbox.pack_start(this.contentAreaLabel, false, false, 2);
    this.contentAreaHbox.pack_start(this.contentAreaEntry, false, false, 2);

    this.actionArea.pack_start(this.actionAreaHbox, true, true, 0);
    this.contentArea.pack_start(this.contentAreaHbox, true, true, 0);
};

var ok_button_cb = function(self, user_data) {
    this.text = this.contentAreaEntry.get_text();
    this.inputDialog.destroy();
    Gtk.main_quit();
}

var cancel_button_cb = function(self, user_data) {
    this.inputDialog.destroy();
    Gtk.main_quit();
}

var dialog_cb = function(self, event, user_data) {
    self.destroy();
    Gtk.main_quit();
}

var input_widget_get_text = function(argc, argv) {
    this.Gtk = imports.gi.Gtk;
    Gtk.init(argc, argv);

    var inputWidget = new InputWidget("PIN");
    inputWidget.actionAreaOkButton.connect("clicked", Lang.bind(inputWidget, ok_button_cb));
    inputWidget.actionAreaCancelButton.connect("clicked", Lang.bind(inputWidget, cancel_button_cb));
    inputWidget.inputDialog.connect("delete-event", Lang.bind(inputWidget, dialog_cb));

    inputWidget.inputDialog.show_all();
    Gtk.main();
    return(inputWidget.text);
}

var printObject = function(object) {
    for(var propName in object) {
	if(object[propName] instanceof Object)
	    printObject(object[propName]);
	else
	    print(propName + " = " + object[propName]);
    }
}

var sampleStreamCb = function(string, userdata) {
    var tweet = JSON.parse(string);
    printObject(tweet);
    print("");
    return(true);
}

var main = function(argc, argv) {
    this.GLib = imports.gi.GLib;
    this.Lang = imports.lang;
    this.Gtweet = imports.gi.Gtweet;

    var tweetObject = new Gtweet.Object();
    if(tweetObject.initkeys()) {
	tweetObject.samplestream(sampleStreamCb, null, null);
    }
    else {
	var authurl = tweetObject.authurl();
	var cmdline = "xdg-open '" + authurl + "'";
	GLib.spawn_command_line_sync(cmdline, null, null, null);
	var pin = input_widget_get_text(argc, argv);
	if(pin) {
	    tweetObject.auth(pin);
	    tweetObject.samplestream(sampleStreamCb, null, null);
	}
    }
}

main(null, null);