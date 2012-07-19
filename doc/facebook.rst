Facebook plugin
===============

Android
-------

1.

You must manually download Facebook Android SDK:
https://github.com/facebook/facebook-android-sdk

::

  cd facebook-android-sdk/facebook/
  path/to/android/sdk/tools/android update project --path . --target android-8
  ant release

In Eclipse add it to your project as a reference project:
Right click your Android project -> Properties -> Android

2.

In your Android project, add to AndroidManifest.xml:

::

  <uses-permission android:name="android.permission.INTERNET" />

3.

From JS, to share something:

::

  jsg.load('scripts/jsg/plugin/facebook.js')
  jsg.facebook.loginOK(->
    jsg.facebook.share(
      link:        'http://www.kayac.com'
      picture:     'http://www.kayac.com/img/common/logo.png'
      name:        'KAYAC Inc.'
      caption:     'Short caption'
      description: 'Long description'
      message:     'Some meaningful message'
    )
  )
  appId = 'xxx'
  jsg.facebook.login(appId)
