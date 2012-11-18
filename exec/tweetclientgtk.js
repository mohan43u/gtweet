#!/usr/bin/env gjs

const Lang = imports.lang;
const GLib = imports.gi.GLib;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gtweet = imports.gi.Gtweet;

const InputField = new Lang.Class({
    Name: "InputField",
    _init: function(text, type) {
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

const create_image = function(data) {
    var memoryInputStream = Gio.MemoryInputStream.new_from_data(data, data.length);
    var pixBuf = GdkPixbuf.Pixbuf.new_from_stream(memoryInputStream, null);
    var image = Gtk.Image.new_from_pixbuf(pixBuf);
    return image;
}

const create_user = function(user) {
    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});

    var create_pair = function(key, value, markup) {
	var hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	var keyLabel = new Gtk.Label({label: key});
	var valueLabel = new Gtk.Label({label: value.toString()});

	keyLabel.set_text(key);
	keyLabel.set_width_chars(10);
	keyLabel.set_alignment(0, 0);

	if(markup)
	{
	    value = value.toString();
	    value = value.replace(/(http[^\s]*)/g,"<a href=\"$1\">$1</a>");
	    value = value.replace(/<a *href="([^"]*)"[^>]*>/g, "<a href=\"$1\">");
	    valueLabel.set_markup(value);
	}
	else
	    valueLabel.set_text(value.toString());
	valueLabel.set_alignment(0, 0);
	valueLabel.set_selectable(true);
	valueLabel.set_line_wrap(true);

	hbox.pack_start(keyLabel, false, false, 5);
	hbox.pack_start(valueLabel, true, true, 0);

	return hbox;
    }
    if(user.verified)
    {
	var verified = create_pair("verified", user.verified);
	vbox.pack_start(verified, false, false, 0);
	verified.show();
    }
    if(user.followers_count){
	var followers = create_pair("followers", user.followers_count);
	vbox.pack_start(followers, false, false, 0);
	followers.show();
    }

    if(user.friends_count)
    {
	var friends = create_pair("friends", user.friends_count);
	vbox.pack_start(friends, false, false, 0);
	friends.show();
    }

    if(user.statuses_count)
    {
	var tweets = create_pair("tweets", user.statuses_count);
	vbox.pack_start(tweets, false, false, 0);
	tweets.show();
    }

    if(user.location)
    {
	var location = create_pair("location", user.location);
	vbox.pack_start(location, false, false, 0);
	location.show();
    }

    if(user.entities
       && user.entities.url
       && user.entities.url.urls[0]
       && user.entities.url.urls[0].url)
    {
	var url = create_pair("url", user.entities.url.urls[0].url, true);
	vbox.pack_start(url, false, false, 0);
	url.show();
    }

    if(user.description)
    {
	var desc = create_pair("description", user.description);
	vbox.pack_start(desc, false, false, 0);
	desc.show();
    }

    return vbox;
}

const create_tweet = function(twitterClient, element) {
    var profileImage = new Gtk.ToggleButton();
    var header = new Gtk.Label();
    var text = new Gtk.Label();
    var user = create_user(element.user);
    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
    var hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});

    profileImage.image = create_image(twitterClient.tweetObject.http(element.user.profile_image_url_https));
    var _toggled = function(self, user) {
	if(user.get_visible())
	    user.set_visible(false);
	else
	    user.show_all();
    }
    profileImage.connect("toggled", Lang.bind(this, _toggled, user));
    
    
    var headerContent = element.user.name + " (" + element.user.screen_name + ")";
    headerContent += (new Date(element.created_at)).toLocaleFormat(" On %a %b %dth, At %I:%M %p,");
    headerContent += " From " + element.source.replace(/<a *href="([^"]*)"[^>]*>/g, "<a href=\"$1\">");
    header.set_markup(headerContent);
    header.set_selectable(true);
    header.set_line_wrap(true);
    header.set_alignment(0, 0);

    text.set_markup(element.text.replace(/(http[^\s]*)/g,"<a href=\"$1\">$1</a>"));
    text.set_selectable(true);
    text.set_line_wrap(true);
    text.set_alignment(0, 0);

    vbox.pack_start(header, false, false, 5);
    vbox.pack_start(text, false, false, 5);
    vbox.pack_start(user, false, false, 5);
    hbox.pack_start(profileImage, false, false, 0);
    hbox.pack_start(vbox, false, false, 10);
    profileImage.show();
    header.show();
    text.show();
    user.hide();
    vbox.show();

    return hbox;
}

var create_radio_tool_button = function(twitterObject, box, stock_id) {
    var icon;
    if(twitterObject.lastIcon == undefined)
	icon = new Gtk.RadioToolButton();
    else
	icon = Gtk.RadioToolButton.new_from_widget(twitterObject.lastIcon);
    icon.set_stock_id(stock_id);

    var _toggled = function(self, box) {
	if(box.get_visible())
	    box.set_visible(false);
	else
	    box.set_visible(true);
    }
    icon.connect("toggled", Lang.bind(twitterObject, _toggled, box));

    twitterObject.toolBar.insert(icon, -1);
    twitterObject.lastIcon = icon;
    icon.show();
}

var FindBox = new Lang.Class({
    Name: "findBox",
    _init: function(text) {
	this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	this.entry = new Gtk.Entry();
	this.entry.set_placeholder_text(text);
	this.button = Gtk.Button.new_from_stock(Gtk.STOCK_FIND);
	this.hbox.pack_start(this.entry, true, true, 0);
	this.hbox.pack_start(this.button, false, false, 0);
	this.entry.show();
	this.button.show();
    },
    get_text: function() {
	return this.entry.get_text();
    }
});

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
    drawToolBar: function() {
	if(this.toolBar == undefined)
	    this.toolBar = new Gtk.Toolbar();
	return this.toolBar;
    },
    drawHomeTimeline: function() {
	if(this.homeBox == undefined)
	{
	    this.homeBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var refreshButton = Gtk.Button.new_from_stock(Gtk.STOCK_REFRESH);
	    this.tweetWindow = new Gtk.ScrolledWindow();
	    this.tweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 5});
	    create_radio_tool_button(this, this.homeBox, Gtk.STOCK_HOME);

	    var _clicked = function(self, userdata){
		this.tweetBox.foreach(function(child, userdata){child.destroy()});
		var jsonText = this.tweetObject.hometimeline(null,
							     null,
							     null);
		var tweetArray = JSON.parse(jsonText);
		var _foreach = function(element, index, array) {
		    var tweet  = create_tweet(this, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetBox.pack_start(tweet, false, false, 0);
		    this.tweetBox.pack_start(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _foreach));
	    }
	    refreshButton.connect("clicked", Lang.bind(this, _clicked));
	    refreshButton.emit("clicked");

	    this.homeBox.pack_start(refreshButton, false, false, 0);
	    this.tweetWindow.add_with_viewport(this.tweetBox);
	    this.homeBox.pack_start(this.tweetWindow, true, true, 0);
	    refreshButton.show();
	    this.tweetBox.show();
	    this.tweetWindow.show();
	}
	return this.homeBox;
    },
    drawTweetSearch: function() {
	if(this.tweetFindBox == undefined)
	{
	    this.tweetFindBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.findbox = new FindBox("type query string here..");
	    this.tweetFindWindow = new Gtk.ScrolledWindow();
	    this.tweetFindTweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 5});
	    create_radio_tool_button(this, this.tweetFindBox, Gtk.STOCK_FIND);

	    var _clicked = function(self, userdata) {
		this.tweetFindTweetBox.foreach(function(child, userdata){child.destroy()});
		var jsonText = this.tweetObject.tweetsearch(this.findbox.get_text(),
							    null,
							    null,
							    null,
							    null,
							    null,
							    null,
							    null,
							    null);
		var jsonObject = JSON.parse(jsonText);
		var tweetArray = jsonObject.statuses;
		var _foreach = function(element, index, array) {
		    var tweet  = create_tweet(this, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetFindTweetBox.pack_start(tweet, false, false, 0);
		    this.tweetFindTweetBox.pack_start(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _foreach));
	    }
	    this.findbox.button.connect("clicked", Lang.bind(this, _clicked));

	    this.tweetFindBox.pack_start(this.findbox.hbox, false, false, 0);
	    this.tweetFindWindow.add_with_viewport(this.tweetFindTweetBox);
	    this.tweetFindBox.pack_start(this.tweetFindWindow, true, true, 0);
	    this.findbox.hbox.show();
	    this.tweetFindTweetBox.show();
	    this.tweetFindWindow.show();
	}
	return this.tweetFindBox;
    },
    getMainBox: function() {
	if(this.mainBox == undefined)
	{
	    this.mainBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.mainBox.pack_start(this.drawToolBar(), false, false, 0);
	    this.mainBox.pack_start(this.drawHomeTimeline(), true, true, 0);
	    this.mainBox.pack_start(this.drawTweetSearch(), true, true, 0);
	    this.drawToolBar().show();
	    this.drawHomeTimeline().show();
	    this.drawTweetSearch().hide();
	}
	return this.mainBox;
    },
    main: function(argc, argv) {
	Gtk.init(argc, argv);
	var twitterClientWindow = new Gtk.Window({title: "twitterClient"});
	var stylecontext = twitterClientWindow.get_style_context();
	var cssProvider = new Gtk.CssProvider();
	var css = "GtkLabel { background-color: blue }";

	cssProvider.load_from_data(css, css.length);
	stylecontext.add_provider(cssProvider, Gtk.GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	twitterClientWindow.set_default_size(500, 800);
	twitterClientWindow.add(this.getMainBox());
	this.getMainBox().show();

	twitterClientWindow.connect("destroy", Gtk.main_quit);
	twitterClientWindow.show();
	Gtk.main();
    }
});

var twitterClient = new TwitterClient();
twitterClient.main(null, null);