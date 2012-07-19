# The stage is not a node. It's not parent of scenes.
class Stage
  # Stage real size is the size of the canvas. See also #updateSize.
  constructor: (@width, @height) ->
    [@canvas, @context] = Sprite.createCanvasAndContext(@width, @height)
    @updateSize(@width, @height)
    @x      = 0
    @y      = 0
    @scenes = []

  add: (scene) ->
    # Current active scene is still visible but not active
    length = @scenes.length
    @scenes[length - 1].active = false if length > 0

    @scenes.push(scene)
    scene.visible = true
    scene.active  = true

  remove: (scene) ->
    length      = @scenes.length
    activeScene = @scenes[length - 1]

    @scenes       = _.without(@scenes, scene)
    scene.visible = false
    scene.active  = false

    # Activate the current top scene if the removed scene was the top scene
    if scene == activeScene && length > 1
      @scenes[length - 2].active = true if length > 0

  # For convenience, the virtual size can be reset by the app by calling
  # jsg.stage.updateSize.
  # The stage real size, which is the size of the canvas, does not change.
  updateSize: (@width, @height) ->
    @scaleX = @width  / Stage.STANDARD_WIDTH
    @scaleY = @height / Stage.STANDARD_HEIGHT

  onFrame: (touchActions, touchXs, touchYs) ->
    try
      activeSceneIndex = @scenes.length - 1
      return if activeSceneIndex < 0

      if touchActions?
        length = touchActions.length
        for i in [0...length]
          action = touchActions[i]
          x      = touchXs[i]
          y      = touchYs[i]
          if action == 0
            jsg.touch.fireDown(x, y)
          else if action == 1
            jsg.touch.fireMove(x, y)
          else if action == 2
            jsg.touch.fireUp(x, y)
          else
            jsg.touch.fireTap(x, y)

      [dt, t] = jsg.fireTick()
      TWEEN.update(t)

      for i in [0..activeSceneIndex]
        scene = @scenes[i]
        if scene?
          scene.fireUpdate(dt, t) if i == activeSceneIndex
          scene.fireDraw(0, 0, 1, 1, 0)
    catch error
      # http://code.google.com/p/v8/wiki/JavaScriptStackTraceApi
      console.log error.stack

  drawToStage: (imageOrCanvas, x, y, width, height, offsetX, offsetY, deg) ->
    # This may happen when using elastic tweening
    return if width < 1 || height < 1

    x += @x
    y += @y

    offsetXLength = width  * offsetX
    offsetYLength = height * offsetY

    # http://stackoverflow.com/questions/4649836/using-html5-canvas-rotate-image-about-arbitrary-point
    ctx = @context
    if -1 < deg < 1
      # No rotation
      ctx.drawImage(imageOrCanvas, x - offsetXLength, y - offsetYLength, width, height)
    else
      ctx.save()
      ctx.translate(x, y)
      ctx.rotate(deg * Math.PI / 180)
      ctx.translate(-offsetXLength, -offsetYLength)
      ctx.drawImage(imageOrCanvas, 0, 0, width, height)
      ctx.restore()

#-------------------------------------------------------------------------------

# Standard screen size if 640 x 960 (iPhone Retina screen)
# The game images should been designed for this screen
Stage.STANDARD_WIDTH  = 640
Stage.STANDARD_HEIGHT = 960
