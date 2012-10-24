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

var GLib = imports.gi.GLib;
var Gio = imports.gi.Gio;
var GTweetObject = imports.gi.GTweetObject;
var tweetObject = new GTweetObject.TweetObject();
if(tweetObject.initkeys()) {
    tweetObject.samplestream(sampleStreamCb, null, null);
}
