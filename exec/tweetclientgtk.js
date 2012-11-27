#!/usr/bin/env gjs

const Lang = imports.lang;
const GLib = imports.gi.GLib;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
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
    show: function() {
	this.hbox.show();
    },
    hide: function() {
	this.hbox.hide();
    },
    destroy: function() {
	this.hbox.destroy();
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

const Image = new Lang.Class({
    Name: "Image",
    _init: function(twitterClient, url, width, height, preserve_aspect_ratio) {
	this.url = url;
	this.width = width;
	this.height = height;
	this.preserve_aspect_ratio = preserve_aspect_ratio;
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    show: function() {
	this.image.show();
    },
    hide: function() {
	this.image.hide();
    },
    destroy: function() {
	this.image.destroy();
    },
    drawImage: function() {
	this.data = this.tweetObject.http(this.url);
	this.memoryInputStream = Gio.MemoryInputStream.new_from_data(this.data, this.data.length);
	this.pixBuf = GdkPixbuf.Pixbuf.new_from_stream_at_scale(this.memoryInputStream,
							       this.width,
							       this.height,
							       this.preserve_aspect_ratio,
							       null,
							       null);
	this.image = Gtk.Image.new_from_pixbuf(this.pixBuf);
	return this.image;
    },
    showAppDialog: function() {
	this.file;
	this.fileIOStream;
	[this.file, this.fileIOStream] = Gio.File.new_tmp(null, this.fileIOStream, null);
	this.fileIOStream.get_output_stream().write(this.data, null);
	
	var dialog = Gtk.AppChooserDialog.new(null, Gtk.DialogFlags.MODAL, this.file);
	var _dialog_response = function(self, response, userdata) {
	    if(response == Gtk.ResponseType.OK)
	    {
		var widget = dialog.get_widget();
		var appInfo = widget.get_app_info();
		appInfo.launch([this.file], null, null);
	    }
	    self.destroy();
	}
	dialog.connect("response", Lang.bind(this, _dialog_response));
	dialog.show();
    }
});

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
	var name = create_pair("name", user.name + "(@" + user.screen_name + ")");
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

const Tweet = new Lang.Class({
    Name: "Tweet",
    _init: function(twitterClient, tweet) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
	this.tweet = tweet;
    },
    show: function() {
	this.box.show();
    },
    hide: function() {
	this.box.hide();
    },
    drawTweet: function() {
	if(this.box == undefined)
	{
	    this.retweetedByBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    if(this.tweet.retweeted_status)
	    {
		var retweetedBy = new Gtk.Label();
		retweetedBy.set_text("Retweeted by, " + this.tweet.user.name + " (@" + this.tweet.user.screen_name + ")");
		retweetedBy.set_selectable(true);
		retweetedBy.set_line_wrap(true);
		retweetedBy.set_alignment(0, 0);
		this.retweetedByBox.pack_start(retweetedBy, false, false, 0);
		retweetedBy.show();
		this.tweet = this.tweet.retweeted_status;
	    }

	    this.box = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.controlBoxVWrap = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.controlBoxHWrap = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.arrowButton = new Gtk.ToggleButton();
	    this.leftArrow = Gtk.Arrow.new(Gtk.ArrowType.LEFT, Gtk.ShadowType.IN);
	    this.rightArrow = Gtk.Arrow.new(Gtk.ArrowType.RIGHT, Gtk.ShadowType.IN);
	    this.userBoxWrap = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.mediaBoxWrap = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.headerBox = new Gtk.Label();
	    this.textBox = new Gtk.Label();


	    var headerContent = this.tweet.user.name + " (@" + this.tweet.user.screen_name + ")";
	    headerContent += (new Date(this.tweet.created_at)).toLocaleFormat(" On %a %b %dth %Y, At %I:%M %p");
	    if(this.tweet.user.location) headerContent += ", From " + this.tweet.user.location;
	    headerContent += ", Using " + this.tweet.source.replace(/<a *href="([^"]*)"[^>]*>/g, "<a href=\"$1\">");
	    this.headerBox.set_markup(headerContent);
	    if(!this.headerBox.get_text()) this.headerBox.set_text(headerContent);
	    this.headerBox.set_selectable(true);
	    this.headerBox.set_line_wrap(true);
	    this.headerBox.set_alignment(0, 0);

	    this.textBox.set_markup(this.tweet.text.replace(/(http[^\s]*)/g,"<a href=\"$1\">$1</a>"));
	    if(!this.textBox.get_text()) this.textBox.set_text(this.tweet.text);
	    this.textBox.set_selectable(true);
	    this.textBox.set_line_wrap(true);
	    this.textBox.set_alignment(0, 0);

	    this.arrowButton.image = this.rightArrow;
	    this.controlBoxHWrap.pack_start(this.arrowButton, false, false, 0);
	    var _arrowButton_toggled = function(self)
	    {
		if(self.get_active())
		{
		    self.image = this.leftArrow;
		    if(this.profileImageBox == undefined)
		    {
			this.profileImageBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
			this.profileImageButton = new Gtk.ToggleButton();
			var url = this.tweet.user.profile_image_url_https;
			this.profileImage = new Image(this.twitterClient, url, -1, -1, true);
			this.profileImageButton.image = this.profileImage.drawImage();
			var _profileImageButton_toggled = function(self) {
			    if(self.get_active())
			    {
				if(this.userBox == undefined)
				{
				    this.userBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
				    this.userBox = create_user(this.tweet.user, true);
				    this.userBoxWrap.pack_start(this.userBox, false, false, 0);
				    this.userBox.show();
				}
				else
				    this.userBox.show();
			    }
			    else
			    {
				if(this.userBox)
				    this.userBox.hide();
			    }
			}
			this.profileImageButton.connect("toggled", Lang.bind(this, _profileImageButton_toggled));
			this.profileImageBox.pack_start(this.profileImageButton, false, false, 0);
			this.controlBoxVWrap.pack_start(this.profileImageBox, false, false, 2);
			this.profileImageButton.show();
			this.profileImageBox.show();
		    }
		    else
			this.profileImageBox.show();

		    if(this.tweet.in_reply_to_status_id_str)
		    {
			if(this.repliedToButton == undefined)
			{
			    this.repliedToButton = new Gtk.ToggleButton({label: "Replied To"});
			    var _repliedToButton_toggled = function(self) {
				if(self.get_active())
				{
				    if(this.repliedToBox == undefined)
				    {
					var jsonText = this.tweetObject.showstatus(this.tweet.in_reply_to_status_id_str);
					var jsonObject = JSON.parse(jsonText);
					if(jsonErrorShow(this.twitterClient, jsonObject)) return;
					var tweet = new Tweet(this.twitterClient, jsonObject);
					this.box.pack_start(tweet.drawTweet(), false, false, 0);
					this.repliedToBox = tweet.box;
					tweet.show();
				    }
				    else
					this.repliedToBox.show();
				}
				else
				{
				    if(this.repliedToBox)
					this.repliedToBox.hide();
				}
			    }
			    this.repliedToButton.connect("toggled", Lang.bind(this, _repliedToButton_toggled));
			    this.controlBoxVWrap.pack_start(this.repliedToButton, false, false, 0);
			    this.repliedToButton.show();
			}
			else
			    this.repliedToButton.show();
		    }

		    if(this.tweet.entities &&
		       this.tweet.entities.media &&
		       this.tweet.entities.media[0] &&
		       this.tweet.entities.media[0].media_url_https)
		    {
			if(this.mediaButton == undefined)
			{
			    this.mediaButton = new Gtk.ToggleButton({label: "Media"});
			    var _mediaButton_toggled = function(self) {
				if(self.get_active())
				{
				    if(this.mediaImage == undefined)
				    {
					var url = this.tweet.entities.media[0].media_url_https
					this.mediaImage = new Image(this.twitterClient, url, 350, 350, true);
					this.mediaImageButton = new Gtk.Button({label: "Open"});
					this.mediaControlBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
					var _mediaImageButton_clicked = function(self) {
					    this.mediaImage.showAppDialog();
					}
					this.mediaImageButton.connect("clicked", Lang.bind(this, _mediaImageButton_clicked));
					this.mediaBoxWrap.pack_start(this.mediaImage.drawImage(), false, false, 0);
					this.mediaControlBox.pack_end(this.mediaImageButton, false, false, 0);
					this.mediaBoxWrap.pack_start(this.mediaControlBox, false, false, 0);
					this.mediaImage.show();
					this.mediaImageButton.show();
					this.mediaControlBox.show();
				    }
				    else
				    {
					this.mediaImage.show();
					this.mediaControlBox.show();
				    }
				}
				else
				{
				    if(this.mediaImage)
				    {
					this.mediaImage.hide();
					this.mediaControlBox.hide();
				    }
				}
			    }
			    this.mediaButton.connect("toggled", Lang.bind(this, _mediaButton_toggled));
			    this.controlBoxVWrap.pack_start(this.mediaButton, false, false, 0);
			    this.mediaButton.show();
			}
			else
			    this.mediaButton.show();
		    }

		    if(this.tweet.entities &&
		       this.tweet.entities.hashtags)
		    {
			if(this.hashTagsBox == undefined)
			{
			    this.hashTagsBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
			    for(var iter = 0; iter <  this.tweet.entities.hashtags.length; iter++)
			    {
				var hashtag = this.tweet.entities.hashtags[iter];
				var hashTagButton = new Gtk.Button({label: "#" + hashtag.text});
				hashTagButton.set_alignment(0, 0);
				var _hashTagButton_clicked = function(self) {
				    this.twitterClient.tweetFind.tweetFindIcon.set_active(true);
				    this.twitterClient.tweetFind.findBox.comboEntry.prepend_text(self.label);
				    this.twitterClient.tweetFind.findBox.comboEntry.set_active(0);
				    this.twitterClient.tweetFind.findBox.button.emit("clicked");
				}
				hashTagButton.connect("clicked", Lang.bind(this, _hashTagButton_clicked));
				this.hashTagsBox.pack_start(hashTagButton, false, false, 0);
				hashTagButton.show();
			    }
			    this.controlBoxVWrap.pack_start(this.hashTagsBox, false, false, 0);
			    this.hashTagsBox.show();
			}
			else
			    this.hashTagsBox.show();
		    }

		    if(this.controlBox == undefined)
		    {
			this.controlBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});

			var timelineButton = new Gtk.Button({label: "Timeline"});
			var _timelineButton_clicked = function(self) {
			    this.twitterClient.userFind.userFindIcon.set_active(true);
			    var id = this.tweet.user.id_str;
			    var name = this.tweet.user.screen_name;
			    this.twitterClient.userFind.findBox.comboEntry.prepend(id, name);
			    this.twitterClient.userFind.findBox.comboEntry.set_active(0);
			    this.twitterClient.userFind.findBox.button.emit("clicked");
			}
			timelineButton.connect("clicked", Lang.bind(this, _timelineButton_clicked));
			this.controlBox.pack_start(timelineButton, false, false, 0);
			timelineButton.show();

			var friendsButton = new Gtk.Button({label: "Friends"});
			var _friendsButton_clicked = function(self) {
			    this.twitterClient.userFind.userFindIcon.set_active(true);
			    this.twitterClient.userFind.userId = this.tweet.user.id_str;
			    this.twitterClient.userFind.iter = 0;
			    this.twitterClient.userFind.isFriends = true;
			    this.twitterClient.userFind.populateUsers();
			    this.twitterClient.userFind.userCombo.set_active(0);
			}
			friendsButton.connect("clicked", Lang.bind(this, _friendsButton_clicked));
			this.controlBox.pack_start(friendsButton, false, false, 0);
			friendsButton.show();

			var followersButton = new Gtk.Button({label: "Followers"});
			var _followersButton_clicked = function(self) {
			    this.twitterClient.userFind.userFindIcon.set_active(true);
			    this.twitterClient.userFind.userId = this.tweet.user.id_str;
			    this.twitterClient.userFind.iter = 0;
			    this.twitterClient.userFind.isFriends = false;
			    this.twitterClient.userFind.populateUsers();
			    this.twitterClient.userFind.userCombo.set_active(0);
			}
			followersButton.connect("clicked", Lang.bind(this, _followersButton_clicked));
			this.controlBox.pack_start(followersButton, false, false, 0);
			followersButton.show();

			if(this.tweet.user.id == this.twitterClient.owner.userObject.id)
			{
			    var deleteButton = new Gtk.Button({label: "Delete"});
			    var _deleteButton_clicked = function(self) {
				var jsonText = this.tweetObject.destroy(this.tweet.id_str);
				var jsonObject = JSON.parse(jsonText);
				if(jsonErrorShow(this.twitterClient, jsonObject)) return;
				this.box.hide();
			    }
			    deleteButton.connect("clicked", Lang.bind(this, _deleteButton_clicked));
			    this.controlBox.pack_start(deleteButton, false, false, 0);
			    deleteButton.show();
			}

			var updateButton = new Gtk.Button({label: "Reply"});
			var _updateButton_clicked = function(self) {
			    this.twitterClient.updateBox.setPostId(this.tweet.id_str);
			    this.twitterClient.updateBox.show();
			}
			updateButton.connect("clicked", Lang.bind(this, _updateButton_clicked));
			this.controlBox.pack_start(updateButton, false, false, 0);
			updateButton.show();

			if(this.tweet.user.id != this.twitterClient.owner.userObject.id)
			{
			    if(this.tweet.user.following)
			    {
				var unfollow = new Gtk.Button({label: "Unfollow"});
				var _unfollow_clicked = function(self) {
				    var jsonText = this.tweetObject.unfollow(null, this.tweet.user.id_str);
				    var jsonObject = JSON.parse(jsonText);
				    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
				    this.status("you are now unfollowing " + this.tweet.user.name);
				}
				unfollow.connect("clicked", Lang.bind(this, _unfollow_clicked));
				this.controlBox.pack_start(unfollow, false, false, 0);
				unfollow.show();
			    }
			    else
			    {
				var follow = new Gtk.Button({label: "Follow"});
				var _follow_clicked = function(self) {
				    var jsonText = this.tweetObject.follow(null, this.tweet.user.id_str);
				    var jsonObject = JSON.parse(jsonText);
				    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
				    this.status("you are now following " + this.tweet.user.name);
				}
				follow.connect("clicked", Lang.bind(this, _follow_clicked));
				this.controlBox.pack_start(follow, false, false, 0);
				follow.show();
			    }

			    var jsonText = this.tweetObject.showstatus(this.tweet.id_str);
			    var jsonObject = JSON.parse(jsonText);
			    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
			    var statusObject = jsonObject;
			    if(statusObject.retweeted)
			    {
				var retweet = new Gtk.Button({label: "Unretweet"});
				var _retweet_clicked = function(self, statusObject) {
				    var jsonText = this.tweetObject.destroy(statusObject.current_user_retweet.id_str);
				    var jsonObject = JSON.parse(jsonText);
				    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
				    this.twitterClient.homeTimeline.refreshButton.emit("clicked");
				}
				retweet.connect("clicked", Lang.bind(this, _retweet_clicked, statusObject));
				this.controlBox.pack_start(retweet, false, false, 0);
				retweet.show();
			    }
			    else
			    {
				var retweet = new Gtk.Button({label: "Retweet"});
				var _retweet_clicked = function(self, statusObject) {
				    var jsonText = this.tweetObject.retweet(statusObject.id_str);
				    var jsonObject = JSON.parse(jsonText);
				    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
				    this.twitterClient.homeTimeline.refreshButton.emit("clicked");
				    this.twitterClient.homeTimeline.homeIcon.set_active(true);
				}
				retweet.connect("clicked", Lang.bind(this, _retweet_clicked, statusObject));
				this.controlBox.pack_start(retweet, false, false, 0);
				retweet.show();
			    }
			}
			this.controlBoxHWrap.pack_start(this.controlBox, false, false, 0);
			this.controlBox.show();
		    }
		    else
			this.controlBox.show();
		}
		else
		{
		    self.image = this.rightArrow;
		    this.profileImageButton.set_active(false);
		    this.profileImageBox.hide();
		    if(this.mediaButton)
		    {
			this.mediaButton.set_active(false);
			this.mediaButton.hide();
		    }
		    if(this.hashTagsBox) this.hashTagsBox.hide();
		    if(this.repliedToButton)
		    {
			this.repliedToButton.set_active(false);
			this.repliedToButton.hide();
		    }
		    this.controlBox.hide();
		}
	    }
	    this.arrowButton.connect("toggled", Lang.bind(this, _arrowButton_toggled));
	    this.arrowButton.show();
	    this.leftArrow.show();
	    this.rightArrow.show();

	    this.vbox.pack_start(this.headerBox, false, false, 2);
	    this.vbox.pack_start(this.retweetedByBox, false, false, 2);
	    this.vbox.pack_start(this.textBox, false, false, 2);
	    this.vbox.pack_start(this.userBoxWrap, false, false, 2);
	    this.vbox.pack_start(this.mediaBoxWrap, false, false, 2);
	    this.headerBox.show();
	    this.retweetedByBox.show();
	    this.textBox.show();
	    this.userBoxWrap.show();
	    this.mediaBoxWrap.show();
	    this.hbox.pack_start(this.controlBoxVWrap, false, false, 2);
	    this.hbox.pack_start(this.vbox, true, true, 2);
	    this.controlBoxVWrap.show();
	    this.vbox.show();
	    this.box.pack_start(this.controlBoxHWrap, false, false, 0);
	    this.box.pack_start(this.hbox, false, false, 0);
	    this.controlBoxHWrap.show();
	    this.hbox.show();

	    return this.box;
	}
	else
	    return this.box;
    }
});

var create_radio_tool_button = function(twitterClient, box, iconName) {
    var icon;
    if(twitterClient.lastIcon == undefined)
	icon = new Gtk.RadioToolButton();
    else
	icon = Gtk.RadioToolButton.new_from_widget(twitterClient.lastIcon, Gtk.IconSize.SMALL_TOOLBAR);
    icon.set_icon_name(iconName);

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
    show: function() {
	this.ownerBox.show();
    },
    hide: function() {
	this.ownerBox.hide();
    },
    destroy: function() {
	this.ownerBox.destroy();
    },
    drawOwner: function() {
	if(this.ownerBox == undefined)
	{
	    this.ownerBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var jsonText = this.tweetObject.usersettings();
	    this.userSettings = JSON.parse(jsonText);
	    if(jsonErrorShow(this.twitterClient, this.userSettings)) return null;

	    var jsonText = this.tweetObject.show(null, this.userSettings.screen_name);
	    this.userObject = JSON.parse(jsonText);
	    if(jsonErrorShow(this.twitterClient, this.userObject)) return null;

	    var imageBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    var vbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var usermini = create_user(this.userObject, false);
	    var userfull = create_user(this.userObject, true);
	    var profileImageBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var profileImageButton = new Gtk.ToggleButton();
	    var controlVbox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    var updateButton = new Gtk.Button({label: "Tweet"});

	    var url = this.userObject.profile_image_url_https;
	    var image = new Image(this.twitterClient, url, -1, -1, true);
	    profileImageButton.image = image.drawImage();
	    var _profileImageButton_toggled = function(self, usermini, userfull) {
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
	    profileImageButton.connect("toggled", Lang.bind(twitterClient, _profileImageButton_toggled, usermini, userfull));
	    profileImageBox.pack_start(profileImageButton, false, false, 0);
	    profileImageButton.show();

	    var _updateButton_clicked = function(self, post_id) {
		this.updateBox.setPostId(post_id);
		this.updateBox.show();
	    }
	    updateButton.connect("clicked", Lang.bind(twitterClient, _updateButton_clicked, null));

	    imageBox.pack_start(profileImageBox, false, false, 5);
	    vbox.pack_start(usermini, false, false, 0);
	    vbox.pack_start(userfull, false, false, 0);
	    controlVbox.pack_start(updateButton, false, false, 0);
	    hbox.pack_start(imageBox, false, false, 0);
	    hbox.pack_start(vbox, true, true, 0);
	    hbox.pack_start(controlVbox, false, false, 0);
	    this.ownerBox.pack_start(hbox, true, true, 0);
	    profileImageBox.show();
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
    show: function() {
	this.homeBox.show();
    },
    hide: function() {
	this.homeBox.hide();
    },
    destroy: function() {
	this.homeBox.destroy();
    },
    drawHomeTimeline: function() {
	if(this.homeBox == undefined)
	{
	    this.homeBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.refreshButton = Gtk.Button.new_from_stock(Gtk.STOCK_REFRESH);
	    this.controlBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.tweetWindow = new Gtk.ScrolledWindow();
	    this.tweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.homeIcon = create_radio_tool_button(this.twitterClient, this.homeBox, "gtk-home");

	    var _refreshButton_clicked = function(self, userdata){
		this.tweetBox.foreach(function(child, userdata){child.destroy()});
		var jsonText = this.tweetObject.hometimeline(null,
							     null,
							     null,
							     null);
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		tweetArray = jsonObject;
		tweetArray.reverse();
		var _foreach = function(element, index, array) {
		    var tweet = new Tweet(this.twitterClient, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetBox.pack_end(tweet.drawTweet(), false, false, 0);
		    this.tweetBox.pack_end(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _foreach));
	    }
	    this.refreshButton.connect("clicked", Lang.bind(this, _refreshButton_clicked));
	    this.refreshButton.emit("clicked");

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

const create_search_button = function() {
    var image = Gtk.Image.new_from_stock(Gtk.STOCK_FIND, Gtk.IconSize.SMALL_TOOLBAR);
    var button = new Gtk.Button();
    button.set_image(image);
    return button;
}

var FindBox = new Lang.Class({
    Name: "findBox",
    _init: function() {
	this.hbox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	this.comboEntry = Gtk.ComboBoxText.new_with_entry();
	this.button = create_search_button();
	this.hbox.pack_start(this.comboEntry, true, true, 0);
	this.hbox.pack_start(this.button, false, false, 0);
	this.comboEntry.show();
	this.button.show();
    },
    show: function() {
	this.hbox.show();
    },
    hide: function() {
	this.hbox.hide();
    },
    destroy: function() {
	this.hbox.destroy();
    },
    get_text: function() {
	return this.comboEntry.get_active_text();
    }
});

const TweetFind = new Lang.Class({
    Name: "TweetFind",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    show: function() {
	this.tweetFindBox.show();
    },
    hide: function() {
	this.tweetFindBox.hide();
    },
    destroy: function() {
	this.tweetFindBox.destroy();
    },
    drawTweetFind: function() {
	if(this.tweetFindBox == undefined)
	{
	    this.tweetFindBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.findBox = new FindBox();
	    this.tweetFindWindow = new Gtk.ScrolledWindow();
	    this.tweetFindTweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 5});
	    this.trendsBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 5});
	    this.countryComboEntry = Gtk.ComboBoxText.new_with_entry();
	    this.countryButton = create_search_button();
	    this.trendsCombo = new Gtk.ComboBoxText();
	    this.trendsButton = create_search_button();
	    this.tweetFindIcon = create_radio_tool_button(this.twitterClient, this.tweetFindBox, "gtk-find");

	    this.countryComboEntry.set_size_request(100, -1);
	    this.countryComboEntry.append_text(this.twitterClient.owner.userSettings.trend_location[0].name);
	    this.countryComboEntry.set_active(0);
	    var _countryButton_clicked = function(self) {
		var jsonText = this.tweetObject.woeid(this.countryComboEntry.get_active_text());
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		var woeid = jsonObject.places.place[0].woeid;

		var jsonText = this.tweetObject.trends(woeid.toString());
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		var trends = jsonObject[0].trends;
		this.trendsCombo.remove_all();
		for(var iter = 0; iter < trends.length; iter++)
		{
		    var trend = trends[iter];
		    this.trendsCombo.append_text(trend.name);
		}
		this.trendsCombo.set_active(0);
	    }
	    this.countryButton.connect("clicked", Lang.bind(this, _countryButton_clicked));

	    var _trendsButton_clicked = function(self) {
		this.findBox.comboEntry.prepend_text(this.trendsCombo.get_active_text());
		this.findBox.comboEntry.set_active(0);
		this.findBox.button.emit("clicked");
	    }
	    this.trendsButton.connect("clicked", Lang.bind(this, _trendsButton_clicked));
	    this.trendsBox.pack_start(this.countryComboEntry, false, false, 0);
	    this.trendsBox.pack_start(this.countryButton, false, false, 0);
	    this.trendsBox.pack_start(this.trendsCombo, true, true, 0);
	    this.trendsBox.pack_start(this.trendsButton, false, false, 0);
	    this.countryComboEntry.show();
	    this.countryButton.show();
	    this.trendsCombo.show();
	    this.trendsButton.show();

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
		    var tweet  = new Tweet(this.twitterClient, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.tweetFindTweetBox.pack_end(tweet.drawTweet(), false, false, 0);
		    this.tweetFindTweetBox.pack_end(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _tweetArray_foreach));
	    }
	    this.findBox.button.connect("clicked", Lang.bind(this, _findBox_button_clicked));

	    this.tweetFindBox.pack_start(this.trendsBox, false, false, 0);
	    this.tweetFindBox.pack_start(this.findBox.hbox, false, false, 0);
	    this.tweetFindWindow.add_with_viewport(this.tweetFindTweetBox);
	    this.tweetFindBox.pack_start(this.tweetFindWindow, true, true, 0);
	    this.trendsBox.show();
	    this.findBox.show();
	    this.tweetFindTweetBox.show();
	    this.tweetFindWindow.show();
	}
	return this.tweetFindBox;
    }
});

const UserFind = new Lang.Class({
    Name: "userFind",
    _init: function(twitterClient) {
	this.twitterClient = twitterClient;
	this.tweetObject = twitterClient.tweetObject;
    },
    show: function() {
	this.userFindBox.show();
    },
    hide: function() {
	this.userFindBox.hide();
    },
    destroy: function() {
	this.userFindBox.destroy();
    },
    populateUsers: function() {
	if(this.iter == 0)
	{
	    this.userCombo.remove_all();
	    this.ids = [];
	}
	print("iter: " + this.iter);
	print("ids.length: " + this.ids.length);
	print("cursor: " + this.cursor);
	print("isFriends: " + this.isFriends);
	if(this.iter >= this.ids.length)
	{
	    if(this.isFriends)
		var jsonText = this.tweetObject.following(this.userId,
							  null,
							  this.cursor);
	    else
		var jsonText = this.tweetObject.followers(this.userId,
							  null,
							  this.cursor);
	    var jsonObject = JSON.parse(jsonText);
	    if(jsonErrorShow(this.twitterClient, jsonObject)) return;
	    if(this.ids == undefined)
		this.ids = jsonObject.ids;
	    else
		this.ids = this.ids.concat(jsonObject.ids);
	    if(jsonObject.next_cursor_str == "0")
		this.cursor = null;
	    else
		this.cursor = jsonObject.next_cursor_str;
	}

	var max = (this.ids.length > 100 ? 100 : this.ids.length)
	var lookupIds = this.ids.slice(this.iter, this.iter + max);
	var jsonText = this.tweetObject.lookup(null, lookupIds.join(","));
	var jsonObject = JSON.parse(jsonText);
	if(jsonErrorShow(this.twitterClient, jsonObject)) return;
	var userArray = jsonObject;
	var _userArray_foreach = function(element, index, array) {
	    this.userCombo.append(element.id_str, element.screen_name);
	}
	userArray.forEach(_userArray_foreach, this);
	this.iter += max;

	if(this.iter < this.ids.length || this.cursor != null)
	    this.userCombo.append("more", "more..");
    },
    drawUserFind: function() {
	if(this.userFindBox == undefined)
	{
	    this.userId = this.twitterClient.owner.userObject.id_str;
	    this.iter = 0;
	    this.ids = [];
	    this.cursor = null;
	    this.isFriends = false;
	    this.userFindBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	    this.userFindIcon = create_radio_tool_button(this.twitterClient, this.userFindBox, "user-info");
	    this.usersBox = new Gtk.Box({orientation: Gtk.Orientation.HORIZONTAL, spacing: 0});
	    this.findBox = new FindBox();
	    this.userCombo = new Gtk.ComboBoxText();
	    this.userComboButton = create_search_button();
	    this.userFindWindow = new Gtk.ScrolledWindow();
	    this.userFindTweetBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 5});

	    this.usersBox.pack_start(this.userCombo, true, true, 0);
	    var _userComboButton_clicked = function(self) {
		var activeText = this.userCombo.get_active_text();
		var activeId = this.userCombo.get_active_id();
		if(activeId == null)
		{
		    this.populateUsers();
		    this.userCombo.set_active(0);
		}
		else if(activeId == "more")
		{
		    var index = this.userCombo.get_active();
		    this.userCombo.remove(index);
		    this.userCombo.set_active(index - 1);
		    this.populateUsers();
		}
		else
		{
		    this.findBox.comboEntry.prepend(activeId, activeText);
		    this.findBox.comboEntry.set_active(0);
		    this.findBox.button.emit("clicked");
		}
	    }
	    this.userComboButton.connect("clicked", Lang.bind(this, _userComboButton_clicked));
	    this.usersBox.pack_start(this.userComboButton, false, false, 0);
	    this.userCombo.show();
	    this.userComboButton.show();

	    var _findBox_button_clicked = function(self, userdata) {
		this.userFindTweetBox.foreach(function(child, userdata){child.destroy()});
		var searchId = this.findBox.comboEntry.get_active_id();
		var searchText = this.findBox.comboEntry.get_active_text();
		var jsonText;
		if(searchId)
		{
		    jsonText = this.tweetObject.usertimeline(searchId,
							     null,
							     null,
							     null,
							     null);
		}
		else
		{
		    jsonText = this.tweetObject.usertimeline(null,
							     searchText,
							     null,
							     null,
							     null);
		}
		var jsonObject = JSON.parse(jsonText);
		if(jsonErrorShow(this.twitterClient, jsonObject)) return;
		var tweetArray = jsonObject;
		tweetArray.reverse();
		var _tweetArray_foreach = function(element, index, array) {
		    var tweet  = new Tweet(this.twitterClient, element);
		    var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		    this.userFindTweetBox.pack_end(tweet.drawTweet(), false, false, 0);
		    this.userFindTweetBox.pack_end(separator, false, false, 0);
		    tweet.show();
		    separator.show();
		}
		tweetArray.forEach(Lang.bind(this, _tweetArray_foreach));
	    }
	    this.findBox.button.connect("clicked", Lang.bind(this, _findBox_button_clicked));

	    this.userFindBox.pack_start(this.usersBox, false, false, 0);
	    this.userFindBox.pack_start(this.findBox.hbox, false, false, 0);
	    this.userFindWindow.add_with_viewport(this.userFindTweetBox);
	    this.userFindBox.pack_start(this.userFindWindow, true, true, 0);
	    this.usersBox.show();
	    this.findBox.show();
	    this.userFindTweetBox.show();
	    this.userFindWindow.show();
	}
	return this.userFindBox;
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
    show: function() {
	this.box.show();
    },
    hide: function() {
	this.box.hide();
    },
    destroy: function() {
	this.box.destroy();
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
	    this.textView.set_size_request(-1, 60);

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
		var tweet = new Tweet(this.twitterClient, jsonObject);
		var separator = new Gtk.Separator({orientation: Gtk.Orientation.HORIZONTAL});
		this.twitterClient.homeTimeline.tweetBox.pack_end(tweet.drawTweet(), false, false, 0);
		this.twitterClient.homeTimeline.tweetBox.pack_end(separator, false, false, 0);
		tweet.show();
		separator.show();
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
	this.statusBox = new Gtk.Box({orientation: Gtk.Orientation.VERTICAL, spacing: 0});
	this.owner = new Owner(this);
	this.updateBox = new UpdateBox(this);
	this.homeTimeline = new HomeTimeline(this);
	this.tweetFind = new TweetFind(this);
	this.userFind = new UserFind(this);
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
	    this.mainBox.pack_start(this.statusBox, false, false, 0);
	    this.mainBox.pack_start(this.owner.drawOwner(), false, false, 0);
	    this.mainBox.pack_start(this.updateBox.drawUpdateBox(), false, false, 0);
	    this.mainBox.pack_start(this.homeTimeline.drawHomeTimeline(), true, true, 0);
	    this.mainBox.pack_start(this.tweetFind.drawTweetFind(), true, true, 0);
	    this.mainBox.pack_start(this.userFind.drawUserFind(), true, true, 0);
	    this.toolBar.show();
	    this.statusBox.show();
	    this.owner.show();
	    this.updateBox.hide();
	    this.homeTimeline.show();
	    this.tweetFind.hide();
	    this.userFind.hide();
	}
	return this.mainBox;
    },
    main: function(argc, argv) {
	Gtk.init(argc, argv);
	this.twitterClientWindow = new Gtk.Window({title: "twitterClient"});
	var stylecontext = this.twitterClientWindow.get_style_context();
	var cssProvider = new Gtk.CssProvider();
	var css = "GtkLabel { background-color: blue }";

	cssProvider.load_from_data(css, css.length);
	stylecontext.add_provider(cssProvider, Gtk.GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	this.twitterClientWindow.set_default_size(500, 800);
	this.twitterClientWindow.add(this.getMainBox());
	this.getMainBox().show();
	this.twitterClientWindow.show();

	this.twitterClientWindow.connect("destroy", Gtk.main_quit);
	Gtk.main();
    }
});

var twitterClient = new TwitterClient();
twitterClient.main(null, null);