class JSG
  createCanvasAndContext: (width, height) ->
    canvas  = new Canvas(width, height)
    context = new Context2d(canvas)

    # See Canvas.prototype.getContext in canvas.js
    canvas._context2d = context
    context.canvas    = canvas

    # https://github.com/LearnBoost/node-canvas
    # Antialias settings other than 'none' give slow/inaccurate result on Android
    context.antialias      = 'none'
    context.patternQuality = 'fast'

    [canvas, context]

  #-----------------------------------------------------------------------------

  loadString: jsgLoadString

  loadJSON: (src) ->
    s = @loadString(src)
    if s then JSON.parse(s) else null

  # arg: jsOrCoffeeScriptFileName
  load: jsgLoad

  ready: (listener) ->
    @readyListeners ?= []
    @readyListeners.push(listener)

  fireReady: (width, height) ->
    [@canvas, @context] = @createCanvasAndContext(width, height)
    if @readyListeners?
      for listener in @readyListeners
        listener(width, height)

  drawLabel: jsgDrawLabel

  #-----------------------------------------------------------------------------

  onFrame: (touchActions, touchXs, touchYs) ->
    @touch.onFrame(touchActions, touchXs, touchYs)
    @fireTick()

  #-----------------------------------------------------------------------------

  # Plugins will be attached here
  plugin: {}

  prefs:
    getString: jsgPrefsGetString
    setString: jsgPrefsSetString
    getInt:    jsgPrefsGetInt
    setInt:    jsgPrefsSetInt

  android:
    callJava: (klass, staticMethod, message) ->
      klass = klass.split('.').join('/')

      if typeof message == 'string'
        jsgCallJava(klass, staticMethod, message)
      else if typeof message == 'object'
        jsgCallJava(klass, staticMethod, JSON.stringify(message))
      else  # undefined
        jsgCallJava(klass, staticMethod)

    backButton: (listener) ->
      @backButtonListeners ?= []
      @backButtonListeners.push(listener)

    fireBackButton: ->
      if @backButtonListeners?
        for listener in @backButtonListeners
          listener()

  quit: ->
    @android.callJava('js.g.JSGActivity', 'quit')

  locale:
    getLanguage: ->
      jsg.android.callJava('js.g.Locale', 'getLanguage')

    getCountry: ->
      jsg.android.callJava('js.g.Locale', 'getCountry')

    getVariant: ->
      jsg.android.callJava('js.g.Locale', 'getVariant')

  alert: (message) ->
    @android.callJava('js.g.Dialog', 'alert', message)

  # Implement window.confirm feature
  # callback: (boolean) ->
  confirm: (message, callback) ->
    callbackId = @callback.register(callback)
    @android.callJava('js.g.Dialog', 'confirm', callbackId + ',' + message)

  # args: canvas, title, description
  saveCanvasToSystemGallery: jsgSaveCanvasToSystemGallery

  saveSpriteToSystemGallery: (sprite, title, description) ->
    jsgSaveCanvasToSystemGallery(sprite.canvas, sprite.width, sprite.height)

  #-----------------------------------------------------------------------------

  pause: (listener) ->
    @pauseListeners ?= []
    @pauseListeners.push(listener)

  firePause: ->
    @paused = true
    return if !@pauseListeners?
    for listener in @pauseListeners
      listener()

  resume: (listener) ->
    @resumeListeners ?= []
    @resumeListeners.push(listener)

  fireResume: ->
    @paused = false
    return if !@resumeListeners?
    for listener in @resumeListeners
      listener()

  # Can be passed as callback to jsg.pause
  pauseAllSound: =>
    @android.callJava('js.g.MediaPlayer', 'pauseAll', '')

  #-----------------------------------------------------------------------------

  # args: fontFamily, src
  fontFace: jsgFontFace

  # http://www.c-sharpcorner.com/UploadFile/72d20e/canvas-text-wrapping-using-html-5/
  # Supports CJK (Chinese, Japanese, Korean - no spaces between characters) and \n
  fillTextWrap: (context, text, x, y, maxWidth, lineHeight) ->
    splitSegments = (paragraph) ->
      SEGMENT_SEPARATOR = [' ']

      ret = []
      sofar = ''

      for c in paragraph
        if c.charCodeAt(0) > 255  # CJK - no spaces between characters
          if sofar.length > 0
            ret.push(sofar)
            sofar = ''
          ret.push(c)
        else
          if c in SEGMENT_SEPARATOR
            if sofar.length > 0
              ret.push(sofar)
              sofar = ''
            ret.push(c)
          else
            sofar += c

      ret.push(sofar) if sofar.length > 0
      ret

    paragraphs = text.split('\n')
    for paragraph in paragraphs
      segments = splitSegments(paragraph)
      line = ''

      for segment in segments
        testLine = line + segment
        metrics = context.measureText(testLine)
        testWidth = metrics.width
        if testWidth > maxWidth
          context.fillText(line, x, y)
          y += lineHeight
          line = segment
        else
          line = testLine

      context.fillText(line, x, y)
      y += lineHeight

#-------------------------------------------------------------------------------

jsg = new JSG

# Global variables should be attached to "window" object
window = window || {}

#-------------------------------------------------------------------------------

jsg.load('scripts/jsg/id_generator.js')
jsg.load('scripts/jsg/callback.js')

jsg.load('scripts/jsg/tick.js')
jsg.load('scripts/jsg/timer.js')

jsg.load('scripts/jsg/sound_pool.js')
jsg.load('scripts/jsg/image_cache.js')

jsg.load('scripts/jsg/touch.js')
