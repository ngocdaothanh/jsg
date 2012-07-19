Overview
========

Unlike other game engines where JavaScript support is an after-thought,
JSG is designed from the start:

* As a JavaScript game engine, thus the API is JavaScript-friendly
* For smartphones (native apps, not web view) and PCs (web apps)

Structure
---------

::

  stage
    scene
      sprite
      node
        sprite
        sprite

There's only one stage.
On the stage there are many scenes, but only one can be active at a time.
Scenes are stacked. They can be pushed in or popped out.
In a scene there are many sprites.
Sprites can be grouped into nodes.

Coodinates
----------

::

  0
  +--------------+--- x
  |              |
  |              |
  |              |
  |              |
  |              |
  |              |
  +--------------+
  |
  |
  y

Included libs
-------------

underscore.js
http://documentcloud.github.com/underscore/

tween.js
https://github.com/sole/tween.js

Memory best practice with sprites and images
--------------------------------------------

To avoid crash because there's not enough memory, cache sprites and reuse
them whenever possible.

Images are already cached by jsg.createImage.
Avoid calling "new Image" whenever possible, call jsg.createImage instead.
