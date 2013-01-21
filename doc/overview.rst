Overview
========

Unlike other game engines where JavaScript support is an after-thought,
JSG is designed from the start:

* As a JavaScript game engine, thus the API is JavaScript-friendly
* For smartphones (native apps, not web view) and PCs (web apps)

Memory best practice with sprites and images
--------------------------------------------

To avoid crash because there's not enough memory, cache sprites and reuse
them whenever possible.

Images are already cached by jsg.createImage.
Avoid calling "new Image" whenever possible, call jsg.createImage instead.
