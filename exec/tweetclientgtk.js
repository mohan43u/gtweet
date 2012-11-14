#!/usr/bin/env gjs

const jsUnit = imports.jsUnit;
const Lang = imports.lang;
const GLib = imports.gi.GLib;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Gtweet = imports.gi.Gtweet;

const InputField = new Lang.Class({
    Name: "InputField",
    _init: function(text) {
	this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 2});
	this.hbox.set_homogeneous(true);
	this.label = new Gtk.Label({label: text});
	this.entry = new Gtk.Entry();
	this.hbox.add(this.label);
	this.hbox.add(this.entry);
    },
    get_text: function() {
	return this.entry.get_text()
    }
});

const show_message = function(message) {
    var dialog = new Gtk.Dialog({title: "twitterClient", type: Gtk.WindowType.TOPLEVEL});
    var label = new Gtk.Label({label: message});
    var contentarea = dialog.get_content_area();
    contentarea.add(label);
    dialog.add_button("Ok", Gtk.ResponseType.OK);
    dialog.show_all();
    dialog.run();
    dialog.destroy();
}

const close_cb = function(self) {
    self.destroy();
    Gtk.main_quit();
}

const consumer_keys_cb = function(self, response_id, consumer_key, consumer_secret) {
    if(response_id == Gtk.ResponseType.OK)
    {
	this.consumer_key = consumer_key.get_text();
	this.consumer_secret = consumer_secret.get_text();
    }
    self.destroy();
    Gtk.main_quit();
}

const get_consumer_keys_from_user = function(twitterClient) {
    var dialog = new Gtk.Dialog({title: "twitterClient", type: Gtk.WindowType.TOPLEVEL});
    var consumer_key = new InputField("consumer_key");
    var consumer_secret = new InputField("consumer_secret");
    var contentarea = dialog.get_content_area();
    contentarea.add(consumer_key.hbox);
    contentarea.add(consumer_secret.hbox);
    dialog.add_button("Ok", Gtk.ResponseType.OK);
    dialog.add_button("Cancel", Gtk.ResponseType.CANCEL);
    dialog.connect("response", Lang.bind(twitterClient, consumer_keys_cb, consumer_key, consumer_secret));
    dialog.connect("close", Lang.bind(twitterClient, close_cb));
    dialog.show_all();
    Gtk.main();
}

const pin_cb = function(self, response_id, pin) {
    if(response_id == Gtk.ResponseType.OK)
	this.pin = pin.get_text();

    self.destroy();
    Gtk.main_quit();
}

const get_pin_from_user = function(twitterClient) {
    var dialog = new Gtk.Dialog({title: "twitterClient", type: Gtk.WindowType.TOPLEVEL});
    var pin = new InputField("pin");
    var contentarea = dialog.get_content_area();
    contentarea.add(pin.hbox);
    dialog.add_button("Ok", Gtk.ResponseType.OK);
    dialog.add_button("Cancel", Gtk.ResponseType.CANCEL);
    dialog.connect("response", Lang.bind(twitterClient, pin_cb, pin));
    dialog.connect("close", Lang.bind(twitterClient, close_cb));
    dialog.show_all();
    Gtk.main();
}

const samplestream_cb = function(twitterObject, res, box) {
// someone need to export g_simple_async_result_get_op_res_gpointer() in gio
// to complete this function
}

const TweetField = new Lang.Class({
    Name: "TweetField",
    _init: function(field, value) {
	var label1 = new Gtk.Label({label: field});
	var label2 = new Gtk.Label({label: value});
	this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	label1.set_width_chars(10);
	label2.set_width_chars(20);
	label1.set_line_wrap(true);
	label2.set_line_wrap(true);
	label1.set_alignment(0, 0);
	label2.set_alignment(0, 0);
	this.hbox.add(label1);
	this.hbox.add(label2);
    }
});

const Tweet = new Lang.Class({
    Name: "Tweet",
    _init: function(element) {
	var createtime = new TweetField("created_at", element["created_at"]);
	var name = new TweetField("name", element.user["name"]);
	var text = new TweetField("text", element["text"]);
	this.vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	this.vbox.add(createtime.hbox);
	this.vbox.add(name.hbox);
	this.vbox.add(text.hbox);
    }
});

const hometimeline_cb = function(element, index, array, box) {
    var tweet = new Tweet(element);
    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
    box.add(tweet.vbox);
    box.add(separator);
    box.show_all();
}

const TwitterClient = new Lang.Class({
    Name: "TwitterClient",
    _init: function() {
	this.tweetObject = new Gtweet.Object();
	if(!this.tweetObject.initkeys())
	{
	    var message = "libgtweet requires you to register a new app with twitter. Here is the steps\n\n";
	    message += "1. Go to https://dev.twitter.com\n";
	    message += "2. Sign-in with your twitter account\n";
	    message += "3. In top left corner, click on your username, a menu will appear\n";
	    message += "4. Select \"My Applications\"\n";
	    message += "5. Click \"create a new application\" button\n";
	    message += "6. Fill the fields except \"Callback URL\" field\n";
	    message += "7. Agree with twitter's terms & conditions and proceed with captcha\n";
	    message += "8. Once you are successfull, select \"Settings\" tab\n";
	    message += "9. Change \"Access Type\" to \"Read,Write and Access direct messages\"\n";
	    message += "10. Update your changes and go to \"Details\" tab\n";
	    message += "11. Copy \"Consumer key\" and \"Consumer Secret\" and paste it in next Dialog box\n\n";
	    message += "Click \"Ok\" to proceed\n\n"
	    show_message(message);
	    get_consumer_keys_from_user(this);
	    if(this.consumer_key && this.consumer_secret)
	    {
		message = "libgtweet requires authorization from you to access you twitter data. Here is the steps\n\n";
		message += "1. You will be redirected to Authorization page\n";
		message += "2. Sign-in with your twitter account, If you already logged-in, then follow next step\n";
		message += "3. Click \"Authorize app\" button\n";
		message += "4. A PIN number will appear, type it in next Dialog box\n\n";
		message += "Click \"Ok\" to proceed\n\n"
		show_message(message);
		var authurl = this.tweetObject.gen_authurl(this.consumer_key, this.consumer_secret)
		GLib.spawn_command_line_sync("xdg-open '" + authurl + "'");
		get_pin_from_user(this);
		if(this.pin)
		    this.tweetObject.auth(this.pin);
	    }
	}
    },
    showSampleStream: function(cancellable, box) {
	this.tweetObject.samplestream(cancellable, samplestream_cb, box);
    },
    showHomeTimeline: function(box) {
	var jsonText = this.tweetObject.hometimeline(null, null, null);
	var tweetArray = JSON.parse(jsonText);
	tweetArray.forEach(Lang.bind(this, hometimeline_cb, box));
    }
});

Gtk.init(null, null);

var twitterClient = new TwitterClient();
var twitterClientWindow = new Gtk.Window({title: "twitterClient"});
var scrolledWindow = new Gtk.ScrolledWindow();
var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 10});
var stylecontext = twitterClientWindow.get_style_context();
var cssProvider = new Gtk.CssProvider();
var css = "GtkLabel { background-color: blue }";

cssProvider.load_from_data(css, css.length);
stylecontext.add_provider(cssProvider, Gtk.GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
twitterClientWindow.set_default_size(500, 800);
twitterClientWindow.connect("destroy", Gtk.main_quit);
scrolledWindow.add_with_viewport(vbox);
twitterClientWindow.add(scrolledWindow);
twitterClientWindow.show_all();
twitterClient.showHomeTimeline(vbox);
Gtk.main();