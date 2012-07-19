Twitter plugin
==============

Android
-------

1.

Do not leave "Callback URL" at the Twitter admin page blank,
otherwise there will be error displaying login dialog.

2.

Copy 5 prebuilt JAR files and twitter_icon.png in jsg_doc/twitter to your
project's libs directory.

twitter_icon.png will be displaed at the login dialog.

3.

In your Android project, add to AndroidManifest.xml:

::

  <uses-permission android:name="android.permission.INTERNET" />

4.

From JS, to share something:

::

  jsg.load('scripts/jsg/plugin/twitter.js')
  jsg.twitter.loginOK(->
    jsg.twitter.share('Some meaningful message')
  )
  consumerKey    = 'xxx'
  consumerSecret = 'yyy'
  jsg.twitter.login(consumerKey, consumerSecret)

Notes about dependencies
------------------------

* com_sugree_twitter_src.jar
  For authentication with Twitter, see how to build below
* An icon in res/drawable directory, the ID must be "twitter_icon"
  Used by com_sugree_twitter_src.jar for displaying at the authentication dialog
* signpost-core-1.2.1.2.jar and signpost-commonshttp4-1.2.1.2.jar
  Used by com_sugree_twitter_src.jar
  Download from http://code.google.com/p/oauth-signpost/downloads/list
* twitter4j-core-android-2.2.5.jar and twitter4j-async-android-2.2.5.jar
  For posting message to Twitter
  Downloaded from http://twitter4j.org/

Build com_sugree_twitter_src.jar
--------------------------------

See https://github.com/sugree/twitter-android-sdk

::

  git clone https://github.com/sugree/twitter-android-sdk.git
  cd twitter-android-sdk/twitter
  rm build.xml
  path/to/android/sdk/tools/android update project --path . --target android-8
  ant release

com_sugree_twitter_src.jar will be created in bin directory.

There are 2 dependency JAR files in the libs directory.
Newer files may be downloaded from:
http://code.google.com/p/oauth-signpost/downloads/list
BUT com_sugree_twitter_src.jar MAY NOT BE COMPATIBLE WITH THEM!
