Native platform interop
=======================

Android
-------

Threading
~~~~~~~~~

The game runs in a thread other than the main UI thread.
When updating UI, you need to wrap your code inside ``JSG.runOnUiThread``
to avoid error ``CalledFromWrongThreadException: Only the original thread that
created a view hierarchy can touch its views``.

::

  JSG.runOnUiThread(new Runnable() {
    // UI updating code goes here
  });

If you want to run code in the game thread, use ``JSG.handler``:

::

  JSG.handler.post(new Runnable() {
    // ...
  });

Call Java from JS
~~~~~~~~~~~~~~~~~

Call static method of a Java class:

::

  jsg.callJava('ClassName', 'staticMethodName1')
  jsg.callJava('ClassName', 'staticMethodName2', aString)

The static method must take one or no string, and return a string (may be null).

::

  public class ClassName {
      public static String staticMethodName1() {
          ...
      }

      public static String staticMethodName2(String aString) {
          ...
      }
  }

You can use JSON to pass data between JS and Java.

In Java, to read JSON:
TODO

Call JS from Java
~~~~~~~~~~~~~~~~~

To schedule the JS code to be run on the game thread as soon as possible:

::

  JSG.postJS("JS code")

Run JS right away if the caller is already currently running on the game thread:

::

  JSG.runJS("JS code")

Avoid JNI error "ReferenceTable overflow (max=512)"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you write JNI plugin, you may hit this runtime error:

::

  ReferenceTable overflow (max=512)
  Failed adding to JNI local ref table (has 512 entries)

To avoid this error, after calling:

::

  jobject obj1 = env->CallObjectMethod(...)
  jobject obj2 = env->CallStativObjectMethod(...)

After working with obj1 and obj2, remember to call:

::

  env->DeleteLocalRef(obj1)
  env->DeleteLocalRef(obj2)

If you want to store the objects for later use, you need to convert them to
JNI global reference:

::

  jobject obj1Global = env->NewGlobalRef(obj1)
  env->DeleteLocalRef(obj1)

For convenience:

::

  jobject obj1Global = JSG::refLocal2Global(obj1)

See also:
http://developer.android.com/guide/practices/design/jni.html
http://groups.google.com/group/android-ndk/browse_thread/thread/4c5e6c6eaf06869b
http://developer.android.com/guide/practices/design/performance.html
