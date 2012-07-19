Sprite
======

Sprites are things displayed on the screen. Sprite has:

* content: an image or an HTML5 canvas
* width, height: the content is always stretched to fill the sprite
* z index

Sprite is not displayed until its content is set.

Transforming sprite
-------------------

Sprite has a transformation matrix. To modify this matrix:

* translate(x, y)
* rotate(degrees, pivotX, pivotY)
* reset()

Creating sprite
---------------

There are several ways to create a sprite:

From an image source.
Usually used when you already know or don't care about the image size.
This is the cheapest way because the texture is cached , should be used whenever possible.

::

  scene  = new Scene
  width  = scene.width / 5
  height = Stage.SCALE  # Proportion of the image will be kept
  sprite = new Sprite(imgSrc, x, y, width, height, z)
  scene.add(sprite)

From an image.
Usually used when you want to load the image, inspecting its size, then use
the size to calculate the sprite size.

::

  img    = new Image(imgSrc)
  width  = image.width  / 2
  height = image.height / 3
  sprite = new Sprite(img, x, y, width, height, z)

From a canvas.
Usually used when you want to compose an image, then set the image to the sprite.
This is the most expensive way, should be avoided whenever possible.
If you just want to draw 2 images one above another, instead of creating a canvas,
drawing the images into it, then setting the canvas to the sprite, you can use
2 sprites one above another.

::

  canvas = new Canvas(100, 100)
  ...
  width  = canvas.width
  height = canvas.height
  sprite = new Sprite(canvas, x, y, width, height, z)
