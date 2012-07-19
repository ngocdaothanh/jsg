AdMob plugin
============

Fundamentals
------------

See https://developers.google.com/mobile-ads-sdk/docs/android/fundamentals

How this plugin works:

1. SMART_BANNER size is used. This size varies depending on real device.
2. When the banner is displayed for the first time, its height is saved to the
   application prefs, then the banner is removed immediately. This time,
   the app is terminated on pause.
3. When the application is started the second time, the stage height is adjusted
   depending on the saved height.

Notes for Android:

1. Make sure you have the latest copy of the Android SDK and that you're compiling
   against at least Android v3.2 (set target in default.properties to android-13).
2. The Google AdMob Ads SDK for Android requires a run-time of Android 1.5 or
   later (set android:minSdkVersion to at least 3 in your AndroidManifest.xml).
   This means you can develop with the latest version of the Android SDK and
   your app will still run on an earlier Android version (1.5 minimum).

Add AdMob for Android
---------------------

Modify AndroidManifest.xml and default.properties as instructed at
`AdMob doc <https://developers.google.com/mobile-ads-sdk/docs/android/fundamentals>`_.
Copy GoogleAdMobAdsSdk-6.0.1.jar to libs directory of your Android project.

Activity example:

::

  import js.g.JSGActivity;
  import js.g.Splash;
  import js.g.Stage;
  import js.g.plugin.AdMob;

  public class MyActivity extends JSGActivity {
      private static final String ADMOB_ID = "My AdMob ID";

      @Override
      public void onCreate(Bundle savedInstanceState) {
          super.onCreate(savedInstanceState);

          View splash = new Splash("images/Default@2x.png");
          View stage  = new Stage(splashView, "scripts/app/scripts/main.js");
          setContentView(stage);

          AdMob.addToBottomIfOnline(ADMOB_ID);
      }
  }

main.coffee example:

::

  jsg.load('scripts/jsg/plugin/netstate.js')
  jsg.load('scripts/jsg/plugin/admob.js')

  jsg.ready((width, height) ->
    if jsg.netstate.isOnline()
      # Adjust stage height
      heightAdMob = jsg.admob.getHeightFromPrefs()
      if heightAdMob?
        height -= heightAdMob
        jsg.stage.updateSize(width, height)
  )
