Instructions to build
=====================

Android NDK is needed, please download and install it:
http://developer.android.com/sdk/ndk/index.html

Prepare source code
-------------------

1.

From the toplevel of the working tree, run:

::

  git submodule init
  git submodule update

Dependency libraries will be downloaded to:

::

  monatsu1-android/jni/png
  monatsu1-android/jni/pixman
  monatsu1-android/jni/freetype2
  monatsu1-android/jni/cairo

Copy:

::

  monatsu1-android/jni/png/scripts/pnglibconf.h.prebuilt

to:

::

  monatsu1-android/jni/png/pnglibconf.h

2.

cpu-features.c and cpu-features.h in Android NDK is needed by Pixman.

::

  cd monatsu1-android/jni
  ln -s path/to/android-ndk/sources/android/cpufeatures

Example:

::

  ln -s ~/opt/android-ndk-r8/sources/android/cpufeatures

3.

If there's building error at the build step (see Build section below),
use the specific version of Pixman and Cairo:

For Pixman:

::

  cd monatsu1-android/jni/pixman
  git checkout refs/tags/pixman-0.25.2

For Cairo:

::

  cd monatsu1-android/jni/cairo
  git checkout refs/tags/1.11.2

To see list of tags:

::

  git tag -l

Build
-----

From this Android project directory (monatsu1-android), run:
path/to/android-ndk/ndk-build

Example:

::

  ~/opt/android-ndk-r8/ndk-build

Native library will be built and copied to libs directory.
After that, build the Java part as normal.

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

See:
http://www.usefullcode.net/2010/12/android_ndklibpng.html
http://blog.livedoor.jp/itahidamito/archives/51676198.html
libpng-1.5.10 should be used if there's any problem

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
Needs Cairo.

Modification:
* Image: Setting "src" will load image from assets directory
* PDF: Removed
* Initialize: Modified to remove Node.js dependency

Backups
~~~~~~~

https://github.com/ngocdaothanh/v8_titanium
https://github.com/ngocdaothanh/android-cairo
https://github.com/ngocdaothanh/node-canvas
