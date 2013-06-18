Build NDK lib
=============

Android NDK is needed, please download and install it:
http://developer.android.com/sdk/ndk/index.html

Prepare source code
-------------------

1.

From the toplevel of the working tree, run:

::

  git submodule update --init

Dependency libraries will be downloaded to:

::

  android/jni/png
  android/jni/pixman
  android/jni/freetype2
  android/jni/cairo

Copy:

::

  android/jni/libpng/scripts/pnglibconf.h.prebuilt

to:

::

  android/jni/libpng/pnglibconf.h

2.

If there's building error at the build step (see Build section below),
use the specific version of Pixman and Cairo:

For Pixman:

::

  cd android/jni/pixman
  git checkout pixman-0.26.2

For Cairo:

::

  cd android/jni/cairo
  git checkout 1.12.14

To see list of tags:

::

  git tag -l

Build
-----

From this Android project directory (android), run:
path/to/android-ndk/ndk-build

Example:

::

  ~/opt/android-ndk-r8e/ndk-build

Native library will be built and copied to libs directory.

Dependency libs
---------------

You should be able to build JSG using the instruction above.
This section lists JSG's dependency libs in more details.

V8 for Android
~~~~~~~~~~~~~~

https://github.com/appcelerator/v8_titanium
Build as is. No modification is needed.

PNG
~~~

libpng-1.6.1 should be used if there's any problem.

See:

* http://www.usefullcode.net/2010/12/android_ndklibpng.html
* http://blog.livedoor.jp/itahidamito/archives/51676198.html

Freetype2
~~~~~~~~~

See:
https://github.com/cdave1/freetype2-android

Cairo
~~~~~

https://github.com/anoek/android-cairo

Build script is modified to build with PNG and Freetype2 support,
so that PNG images and custom fonts can be loaded and drawn.

pixman.mk has been modified:

* LOCAL_CFLAGS:  added -Ijni/cpufeatures and -include "limits.h"
* LIBPIXMAN_SRC: added pixman/pixman/pixman-noop.c and cpufeatures/cpu-features.c

cpufeatures in Android NDK is needed by Pixman (see above section).

Canvas for Node.js
~~~~~~~~~~~~~~~~~~

https://github.com/LearnBoost/node-canvas

Cairo is needed to build.

Modification:
* Image: Setting "src" will load image from assets directory
* PDF: Removed
* Initialize: Modified to remove Node.js dependency

Backups
~~~~~~~

https://github.com/ngocdaothanh/v8_titanium
https://github.com/ngocdaothanh/android-cairo
https://github.com/ngocdaothanh/node-canvas

Build Java lib
==============

After building the NDK lib, build the Java part as normal:
* See project.properties to know dependency projects
* Also see doc directory to know dependency .jar files

Build JS
========

::

  coffee --compile --bare --watch --output js_build/ js/
