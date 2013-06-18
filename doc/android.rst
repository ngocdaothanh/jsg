Android
=======

Directory structure
-------------------

::

  coffee               CoffeeScript source code in (if you write game in CoffeeScript)
    main.coffee
    foo.coffee
  scripts
    app                Compiled from "coffee" above
      main.js
      foo.js
    jsg
      [JSG .js files]
    covas
      [Covas .js files]
  assets
    scripts            Symlink to "scripts" above in during development.
                       Delete this symlink when release.
    images
  src                  Java source code

Compile CoffeeScript files in bare mode:

::

  coffee -cbwo scripts/app/scripts app
  coffee -cbwo scripts/jsg jsg/src

Create main activity
--------------------

1. Extend JSGActivity
2. Create splash, you may use js.g.Splash or your own
3. Create stage and set the splash to it
4. Add the stage to the activity

::

  import android.os.Bundle;
  import android.view.View;

  import js.g.JSGActivity;
  import js.g.Splash;
  import js.g.Stage;

  public class MyActivity extends JSGActivity {
      @Override
      public void onCreate(Bundle savedInstanceState) {
          super.onCreate(savedInstanceState);

          // splash.png is in assets/images directory
          View splash = new Splash("images/splash.png");
          View stage  = new Stage(splash, "scripts/js/main.coffee");
          setContentView(stage);
      }
  }

Features only available on Android
----------------------------------

jsg.android.callJava: See :doc:`NativeNative platform interop </native>`.

jsg.android.backButton:

::

  jsg.android.backButton(->
    jsg.confirm(
      'Do you want to quit?',
      (isOK)->
        jsg.quit() if isOK
    )
  )
