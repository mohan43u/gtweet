#!/usr/bin/env gjs
// after compilation, execute this program with
// LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:$(pwd)" GI_TYPELIB_PATH="${GI_TYPELIB_PATH}:$(pwd)" ./tweet.js

printObject = function(object) {
    for(var propName in object) {
	if(object[propName] instanceof Object)
	    printObject(object[propName]);
	else
	    print(propName + " = " + object[propName]);
    }
}

sampleStreamCb = function(string, userdata) {
    var tweet = JSON.parse(string);
    printObject(tweet);
    print("");
    return(true);
}

responseCb = function(self, event) {
    printerr("hello world" + self + " " + event);
}

getPin = function() {
    var GObject = imports.gi.GObject;
    var Gtk = imports.gi.Gtk;
    Gtk.init(null, null);

    Gdk.threads_enter();
    var inputDialog = new Gtk.Dialog();
    inputDialog.connect("event", responseCb);
    inputDialog.add_button(Gtk.STOCK_OK, Gtk.ResponseType.OK);
    inputDialog.add_button(Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL);

    var contentAreaBox = inputDialog.get_content_area();
    var contentAreaHbox = new Gtk.HBox({homogeneous:false, spacing:10});
    var pinLabel = new Gtk.Label({label:"PIN: "});
    var pinEntry = new Gtk.Entry();
    contentAreaHbox.pack_start(pinLabel, false, false, 2);
    contentAreaHbox.pack_start(pinEntry, false, false, 2);
    contentAreaBox.pack_start(contentAreaHbox, true, true, 0);
    contentAreaBox.show_all();

    var pin = null;
    var result = inputDialog.run();
    if(result === Gtk.ResponseType.OK) {
	pin = pinEntry.get_text();
    }

    inputDialog.destroy();
    return(pin);
}

var GLib = imports.gi.GLib;
var Gtweet = imports.gi.Gtweet;

var tweetObject = new Gtweet.Object();
if(tweetObject.initkeys()) {
    tweetObject.samplestream(sampleStreamCb, null, null);
}
else {
    var authurl = tweetObject.authurl();
    var cmdline = "xdg-open '" + authurl + "'";
    GLib.spawn_command_line_sync(cmdline, null, null, null);
    var pin = getPin();
    if(pin) {
	tweetObject.auth(pin);
	tweetObject.samplestream(sampleStreamCb, null, null);
    }
}
