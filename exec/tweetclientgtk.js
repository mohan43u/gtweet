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

const dialogbox = function(message) {
    var dialog = new Gtk.Dialog({title: "twitterClient", type: Gtk.WindowType.TOPLEVEL});
    var label = new Gtk.Label({label: message});
    var contentarea = dialog.get_content_area();
    contentarea.add(label);
    dialog.add_button("Ok", Gtk.ResponseType.OK);
    dialog.show_all();
    dialog.grab_focus();
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
    keyLabel.show();
    valueLabel.show();

    return hbox;
}

const create_user = function(user, isfull) {
    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});

    if(user.name && user.screen_name)
    {
	var name = create_pair("name", user.name + "(" + user.screen_name + ")");
	vbox.pack_start(name, false, false, 0);
	name.show();
    }

    if(isfull && user.created_at)
    {
	var createtime = create_pair("createtime", (new Date(user.created_at)).toLocaleString());
	vbox.pack_start(createtime, false, false, 0);
	createtime.show();
    }

    if(user.verified)
    {
	var verified = create_pair("verified", user.verified);
	vbox.pack_start(verified, false, false, 0);
	verified.show();
    }

    if(isfull && user.time_zone)
    {
	var tz = create_pair("time_zone", user.time_zone);
	vbox.pack_start(tz, false, false, 0);
	tz.show();
    }

    if(isfull && user.followers_count){
	var followers = create_pair("followers", user.followers_count);
	vbox.pack_start(followers, false, false, 0);
	followers.show();
    }

    if(isfull && user.friends_count)
    {
	var friends = create_pair("friends", user.friends_count);
	vbox.pack_start(friends, false, false, 0);
	friends.show();
    }

    if(isfull && user.statuses_count)
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

    if(isfull && user.description)
    {
	var desc = create_pair("description", user.description);
	vbox.pack_start(desc, false, false, 0);
	desc.show();
    }

    return vbox;
}

const jsonErrorShow = function(twitterClient, jsonObject) {
    if(jsonObject.errors)
    {
	if(typeof(jsonObject.errors) == "string")
	{
	    twitterClient.status(jsonObject.errors);
	    return 1;
	}
	if(typeof(jsonObject.errors) == "object")
	{
	    var message = jsonObject.errors[0].message + "(" + jsonObject.errors[0].code + ")"; 
	    twitterClient.status(message);
	    throw new Error(message);
	}
    }
    return 0;
}

const create_tweet = function(twitterClient, element_orig) {
    var hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});

    if(element_orig.retweeted_status)
	var element = element_orig.retweeted_status;
    else
	var element = element_orig;

    var header = new Gtk.Label();
    var text = new Gtk.Label();
    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
    var controlVbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
    var retweetedby = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
    var user = create_user(element.user, true);
    var controlHbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});

    var profileImage = new Gtk.ToggleButton();
    var _profileImage_show = function(self, twitterClient) {
	self.image = create_image(twitterClient.tweetObject.http(element.user.profile_image_url_https));
    }
    profileImage.connect("show", Lang.bind(this, _profileImage_show, twitterClient));
    var _profileImage_toggled = function(self, user) {
	if(self.get_active())
	    user.show();
	else
	    user.hide();
    }
    profileImage.connect("toggled", Lang.bind(twitterClient, _profileImage_toggled, user));
    controlVbox.pack_start(profileImage, false, false, 5);
    profileImage.show();

    var headerContent = element.user.name + " (" + element.user.screen_name + ")";
    headerContent += (new Date(element.created_at)).toLocaleFormat(" On %a %b %dth, At %I:%M %p");
    if(element.place) headerContent += ", From " + element.place.name;
    headerContent += ", Using " + element.source.replace(/<a *href="([^"]*)"[^>]*>/g, "<a href=\"$1\">");
    header.set_markup(headerContent);
    header.set_selectable(true);
    header.set_line_wrap(true);
    header.set_alignment(0, 0);

    var follow = new Gtk.Button({label: "Follow"});
    var _follow_clicked = function(self, element) {
	jsonText = this.tweetObject.follow(null, element.user.id_str);
	jsonObject = JSON.parse(jsonText);
	if(jsonErrorShow(this, jsonObject)) return;
	this.status("you are now following " + element.user.name);
    }
    follow.connect("clicked", Lang.bind(twitterClient, _follow_clicked, element));
    controlHbox.pack_end(follow, false, false, 0);
    follow.show();
    
    var unfollow = new Gtk.Button({label: "Unfollow"});
    var _unfollow_clicked = function(self, element) {
	jsonText = this.tweetObject.unfollow(null, element.user.id_str);
	jsonObject = JSON.parse(jsonText);
	if(jsonErrorShow(this, jsonObject)) return;
	this.status("you are now unfollowing " + element.user.name);
    }
    unfollow.connect("clicked", Lang.bind(twitterClient, _unfollow_clicked, element));
    controlHbox.pack_end(unfollow, false, false, 0);
    unfollow.show();
    
    var retweet = new Gtk.Button({label: "Retweet"});
    var _retweet_clicked = function(self, element) {
	jsonText = this.tweetObject.retweet(element.id_str);
	jsonObject = JSON.parse(jsonText);
	if(jsonErrorShow(this, jsonObject)) return;
	var tweet  = create_tweet(this, element);
	var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
	this.homeTimeline.tweetBox.pack_end(tweet, false, false, 0);
	this.homeTimeline.tweetBox.pack_end(separator, false, false, 0);
	tweet.show();
	separator.show();
	this.homeTimeline.homeIcon.set_active(true);
    }
    retweet.connect("clicked", Lang.bind(twitterClient, _retweet_clicked, element));
    controlHbox.pack_end(retweet, false, false, 0);
    retweet.show();

    var updateButton = new Gtk.Button({label: "Reply"});
    var _updateButton_clicked = function(self, post_id) {
	this.updateBox.setPostId(post_id);
	this.updateBox.box.show();
    }
    updateButton.connect("clicked", Lang.bind(twitterClient, _updateButton_clicked, element.id_str));
    controlHbox.pack_end(updateButton, false, false, 0);
    updateButton.show();


    if(element.user.id == twitterClient.owner.userObject.id)
    {
	var deleteButton = new Gtk.Button({label: "Delete"});
	var _deleteButton_clicked = function(self, element, hbox) {
	    jsonText = this.tweetObject.destroy(element.id_str);
	    jsonObject = JSON.parse(jsonText);
	    if(jsonErrorShow(this, jsonObject)) return;
	    hbox.hide();
	}
	deleteButton.connect("clicked", Lang.bind(twitterClient, _deleteButton_clicked, element, hbox));
	controlHbox.pack_end(deleteButton, false, false, 0);
	deleteButton.show();
    }

    if(element.id != element_orig.id)
    {
	var rlabel = new Gtk.Label();
	rlabel.set_text("Retweeted by, " + element_orig.user.name + " (" + element_orig.user.screen_name + ")");
	rlabel.set_width_chars(10);
	rlabel.set_alignment(0, 0);
	retweetedby.pack_start(rlabel, false, false, 0);
	rlabel.show();
    }

    text.set_markup(element.text.replace(/(http[^\s]*)/g,"<a href=\"$1\">$1</a>"));
    text.set_selectable(true);
    text.set_line_wrap(true);
    text.set_alignment(0, 0);

    hbox.pack_start(controlVbox, false, false, 5);
    vbox.pack_start(controlHbox, false, false, 5);
    vbox.pack_start(header, false, false, 5);
    vbox.pack_start(retweetedby, false, false, 5);
    vbox.pack_start(text, false, false, 5);
    vbox.pack_start(user, false, false, 5);
    hbox.pack_start(vbox, false, false, 5);
    controlVbox.show();
    controlHbox.show();
    header.show();
    retweetedby.show();
    text.show();
    user.hide();
    vbox.show();

    return hbox;
}

var create_radio_tool_button = function(twitterClient, box, stock_id) {
    var icon;
    if(twitterClient.lastIcon == undefined)
	icon = new Gtk.RadioToolButton();
    else
	icon = Gtk.RadioToolButton.new_from_widget(twitterClient.lastIcon);
    icon.set_stock_id(stock_id);

    var _icon_toggled = function(self, box) {
	if(self.get_active())
	    box.show();
	else
	    box.hide();
    }
    icon.connect("toggled", Lang.bind(twitterClient, _icon_toggled, box));

    twitterClient.toolBar.insert(icon, -1);
    twitterClient.lastIcon = icon;
    icon.show();
    return icon;
}

const Owner = new Lang.Class({
    Name: "Owner",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    drawOwner: function() {
	if(this.ownerBox == undefined)
	{
	    this.ownerBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var jsonText = this.tweetObject.usersettings();
	    this.userSettings = JSON.parse(jsonText);
	    if(jsonErrorShow(this.twitterClient, this.userSettings)) return null;

	    jsonText = this.tweetObject.show(null, this.userSettings.screen_name);
	    this.userObject = JSON.parse(jsonText);
	    if(jsonErrorShow(this.twitterClient, this.userObject)) return null;

	    var imageBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var usermini = create_user(this.userObject, false);
	    var userfull = create_user(this.userObject, true);
	    var profileImage = new Gtk.ToggleButton();
	    var controlVbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var updateButton = new Gtk.Button({label: "Tweet"});

	    var _profileImage_show = function(self, twitterClient) {
		self.image = create_image(twitterClient.tweetObject.http(this.userObject.profile_image_url_https));
	    }
	    profileImage.connect("show", Lang.bind(this, _profileImage_show, this.twitterClient));
	    var _profileImage_toggled = function(self, usermini, userfull) {
		if(self.get_active())
		{
		    usermini.hide();
		    userfull.show();
		}
		else
		{
		    usermini.show();
		    userfull.hide();
		}
	    }
	    profileImage.connect("toggled", Lang.bind(twitterClient, _profileImage_toggled, usermini, userfull));

	    var _updateButton_clicked = function(self, post_id) {
		this.updateBox.setPostId(post_id);
		this.updateBox.box.show();
	    }
	    updateButton.connect("clicked", Lang.bind(twitterClient, _updateButton_clicked, null));

	    imageBox.pack_start(profileImage, false, false, 5);
	    vbox.pack_start(usermini, false, false, 0);
	    vbox.pack_start(userfull, false, false, 0);
	    controlVbox.pack_start(updateButton, false, false, 0);
	    hbox.pack_start(imageBox, false, false, 0);
	    hbox.pack_start(vbox, true, true, 0);
	    hbox.pack_start(controlVbox, false, false, 0);
	    this.ownerBox.pack_start(hbox, true, true, 0);
	    profileImage.show();
	    usermini.show();
	    userfull.hide();
	    updateButton.show();
	    imageBox.show();
	    vbox.show();
	    controlVbox.show();
	    hbox.show();
	}
	return this.ownerBox;
    }
});

const HomeTimeline = new Lang.Class({
    Name: "HomeTimeline",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    drawHomeTimeline: function() {
	if(this.homeBox == undefined)
	{
	    this.homeBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.refreshButton = Gtk.Button.new_from_stock(Gtk.STOCK_REFRESH);
	    this.controlBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.tweetWindow = new Gtk.ScrolledWindow();
	    this.tweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.homeIcon = create_radio_tool_button(this.twitterClient, this.homeBox, Gtk.STOCK_HOME);

	    var _refreshButton_clicked = function(self, userdata){
		this.tweetBox.foreach(function(child, userdata){child.destroy()});
		var jsonText = this.tweetObject.hometimeline(null,
							     null,
							     null);
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		tweetArray = jsonObject;
		tweetArray.reverse();
		var _foreach = function(element, index, array) {
		    var tweet = create_tweet(this.twitterClient, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetBox.pack_end(tweet, false, false, 0);
		    this.tweetBox.pack_end(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _foreach));
	    }
	    this.refreshButton.connect("clicked", Lang.bind(this, _refreshButton_clicked));
	    this.refreshButton.clicked();

	    this.controlBox.pack_start(this.refreshButton, true, true, 0);
	    this.homeBox.pack_start(this.controlBox, false, false, 0);
	    this.tweetWindow.add_with_viewport(this.tweetBox);
	    this.homeBox.pack_start(this.tweetWindow, true, true, 0);
	    this.refreshButton.show();
	    this.controlBox.show();
	    this.tweetWindow.show();
	    this.tweetBox.show();
	}
	return this.homeBox;
    }
});

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

const TweetSearch = new Lang.Class({
    Name: "TweetSearch",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    drawTweetSearch: function() {
	if(this.tweetFindBox == undefined)
	{
	    this.tweetFindBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.findBox = new FindBox("type query string here..");
	    this.tweetFindWindow = new Gtk.ScrolledWindow();
	    this.tweetFindTweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 5});
	    this.tweetFindIcon = create_radio_tool_button(this.twitterClient, this.tweetFindBox, Gtk.STOCK_FIND);

	    var _findBox_button_clicked = function(self, userdata) {
		this.tweetFindTweetBox.foreach(function(child, userdata){child.destroy()});
		var jsonText = this.tweetObject.tweetsearch(this.findBox.get_text(),
							    null,
							    null,
							    null,
							    null,
							    null,
							    null,
							    null,
							    null);
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		var tweetArray = jsonObject.statuses;
		tweetArray.reverse();
		var _tweetArray_foreach = function(element, index, array) {
		    var tweet  = create_tweet(this.twitterClient, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetFindTweetBox.pack_end(tweet, false, false, 0);
		    this.tweetFindTweetBox.pack_end(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _tweetArray_foreach));
	    }
	    this.findBox.button.connect("clicked", Lang.bind(this, _findBox_button_clicked));

	    this.tweetFindBox.pack_start(this.findBox.hbox, false, false, 0);
	    this.tweetFindWindow.add_with_viewport(this.tweetFindTweetBox);
	    this.tweetFindBox.pack_start(this.tweetFindWindow, true, true, 0);
	    this.findBox.hbox.show();
	    this.tweetFindTweetBox.show();
	    this.tweetFindWindow.show();
	}
	return this.tweetFindBox;
    }
});

const UpdateBox = new Lang.Class({
    Name: "UpdateBox",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    setPostId: function(post_id) {
	this.post_id = post_id;
    },
    drawUpdateBox: function() {
	if(this.box == undefined)
	{
	    this.box = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.textView = new Gtk.TextView();
	    this.textBuffer = this.textView.get_buffer();
	    this.fileChooserButton = Gtk.FileChooserButton.new("", Gtk.FileChooserAction.OPEN);
	    this.ok = Gtk.Button.new_from_stock(Gtk.STOCK_OK);
	    this.cancel = Gtk.Button.new_from_stock(Gtk.STOCK_CANCEL);

	    this.textView.set_wrap_mode(Gtk.WrapMode.WORD);
	    this.textView.set_size_request(-1, 50);

	    var _ok_clicked = function(self, post_id) {
		var startIter, endIter;
		[startIter, endIter] = this.textBuffer.get_bounds();
		var updateText = this.textBuffer.get_text(startIter, endIter, true);
		var filename = this.fileChooserButton.get_filename();
		if(filename)
		{
		    var jsonText = this.tweetObject.updatemedia(updateText, filename, this.post_id);
		    var jsonObject = JSON.parse(jsonText);
		    if(jsonErrorShow(this, jsonObject)) return;
		}
		else
		{
		    var jsonText = this.tweetObject.update(updateText, this.post_id);
		    var jsonObject = JSON.parse(jsonText);
		    if(jsonErrorShow(this, jsonObject)) return;
		}
		var tweet  = create_tweet(this.twitterClient, jsonObject);
		var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		this.twitterClient.homeTimeline.tweetBox.pack_end(tweet, false, false, 0);
		this.twitterClient.homeTimeline.tweetBox.pack_end(separator, false, false, 0);
		tweet.show();
		separator.show();
		this.twitterClient.homeTimeline.homeIcon.set_active(true);
		this.cancel.clicked();
	    }
	    this.ok.connect("clicked", Lang.bind(this, _ok_clicked, this.post_id));

	    var _cancel_clicked = function(self) {
		var startIter, endIter;
		[startIter, endIter] = this.textBuffer.get_bounds();
		this.textBuffer.delete(startIter, endIter);
		this.box.hide();
	    }
	    this.cancel.connect("clicked", Lang.bind(this, _cancel_clicked));

	    this.box.pack_start(this.textView, true, true, 0);
	    this.hbox.pack_start(this.fileChooserButton, true, true, 0);
	    this.hbox.pack_start(this.ok, false, false, 0);
	    this.hbox.pack_start(this.cancel, false, false, 0);
	    this.box.pack_start(this.hbox, false, false, 0);
	    this.textView.show();
	    this.fileChooserButton.show();
	    this.ok.show();
	    this.cancel.show();
	    this.hbox.show();
	}

	return this.box;
    }
});

const TwitterClient = new Lang.Class({
    Name: "TwitterClient",
    _init: function() {
	this.tweetObject = new Gtweet.Object();
	this.toolBar = new Gtk.Toolbar();
	this.owner = new Owner(this);
	this.statusBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	this.updateBox = new UpdateBox(this);
	this.homeTimeline = new HomeTimeline(this);
	this.tweetSearch = new TweetSearch(this);
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
	    dialogbox(message);
	    get_consumer_keys_from_user(this);
	    if(this.consumer_key && this.consumer_secret)
	    {
		message = "libgtweet requires authorization from you to access you twitter data. Here is the steps\n\n";
		message += "1. You will be redirected to Authorization page\n";
		message += "2. Sign-in with your twitter account, If you already logged-in, then follow next step\n";
		message += "3. Click \"Authorize app\" button\n";
		message += "4. A PIN number will appear, type it in next Dialog box\n\n";
		message += "Click \"Ok\" to proceed\n\n"
		dialogbox(message);
		var authurl = this.tweetObject.gen_authurl(this.consumer_key, this.consumer_secret)
		GLib.spawn_command_line_sync("xdg-open '" + authurl + "'");
		get_pin_from_user(this);
		if(this.pin)
		    this.tweetObject.auth(this.pin);
	    }
	}
    },
    status: function(message) {
	var vbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	var msg = new Gtk.Label({label: message});
	var ok = Gtk.Button.new_from_stock(Gtk.STOCK_OK);

	msg.set_alignment(0, 0);
	var _clicked = function(self) {
	    this.destroy();
	}
	ok.connect("clicked", Lang.bind(vbox, _clicked));

	vbox.pack_start(msg, true, true, 5);
	vbox.pack_start(ok, false, false, 5)
	msg.show();
	ok.show();
	vbox.show();
	this.statusBox.pack_start(vbox, false, false, 0);
    },
    getMainBox: function() {
	if(this.mainBox == undefined)
	{
	    this.mainBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.mainBox.pack_start(this.toolBar, false, false, 0);
	    this.mainBox.pack_start(this.owner.drawOwner(), false, false, 0);
	    this.mainBox.pack_start(this.statusBox, false, false, 0);
	    this.mainBox.pack_start(this.updateBox.drawUpdateBox(), false, false, 0);
	    this.mainBox.pack_start(this.homeTimeline.drawHomeTimeline(), true, true, 0);
	    this.mainBox.pack_start(this.tweetSearch.drawTweetSearch(), true, true, 0);
	    this.toolBar.show();
	    this.owner.ownerBox.show();
	    this.statusBox.show();
	    this.updateBox.box.hide();
	    this.homeTimeline.homeBox.show();
	    this.tweetSearch.tweetFindBox.hide();
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
	twitterClientWindow.show();

	twitterClientWindow.connect("destroy", Gtk.main_quit);
	Gtk.main();
    }
});

var twitterClient = new TwitterClient();
twitterClient.main(null, null);