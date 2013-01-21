# Some browsers does not have console.log
if !window.console?
  window.console =
    log: ->
      # Ignore

#-------------------------------------------------------------------------------

jsgJSONs = {}

jsgLoadJSON = (fileName) ->
  jsgJSONs[fileName]

#-------------------------------------------------------------------------------

jsgLoad = (jsFileName) ->
  s = document.createElement('script')
  s.type = 'text/javascript'
  s.src  = jsFileName

  head = document.getElementsByTagName('head')[0]
  head.appendChild(s)

jsgCreateCanvasAndContext = (width, height) ->
  canvas = document.createElement('canvas')
  ctx    = canvas.getContext('2d')
  canvas.context = ctx
  canvas.width   = width
  canvas.height  = height
  [canvas, ctx]

jsgDrawLabel = (canvas, context, fontName, fontSize, text, textColor, alignx, aligny) ->
  context.clearRect(0, 0, canvas.width, canvas.height)

  if fontName != null
    context.font = "#{fontSize}px '#{fontName}'"
  else
    context.font = "#{fontSize}px sans-serif"

  context.fillStyle = textColor

  [context.textAlign, x] = switch alignx
    when Label.CENTER then ['center', canvas.width / 2]
    when Label.LEFT   then ['left',   0]
    when Label.RIGHT  then ['right',  canvas.width]

  [context.textBaseline, y] = switch aligny
    when Label.CENTER then ['middle', canvas.height / 2]
    when Label.TOP    then ['top',    0]
    when Label.BOTTOM then ['bottom', canvas.height]

  jsg.fillTextWrap(context, text, x, y, canvas.width, fontSize)

jsgPrefsGetString = (key) ->
  null

jsgPrefsSetString = (key, value) ->

jsgPrefsGetInt = (key) ->
  null

jsgPrefsSetInt = (key, value) ->

jsgAlert = (message) ->
  alert(message)

jsgConfirm = (message, callback) ->
  ret = confirm(message)
  callback(ret)

jsgSaveCanvasToSystemGallery = (canvas, title, description) ->

# http://stackoverflow.com/questions/2756575/drawing-text-to-canvas-with-font-face-does-not-work-at-the-first-time
jsgFontFace = (fontFamily, src) ->
  $('body').append('<div style="font-family: \' + fontFamily + \'">.</div>')

jsgLocale =
  getLanguage: ->
    language = window.navigator.userLanguage || window.navigator.language
    language.split('-')[0]

  getCountry: ->
    language = window.navigator.userLanguage || window.navigator.language
    language.split('-')[1] || ''

  getVariant: ->
    language = window.navigator.userLanguage || window.navigator.language
    language.split('-')[1] || ''

#-------------------------------------------------------------------------------

getEventElementXY = (event) ->
  target = event.target
  offset = $(target).offset()
  left   = offset.left
  top    = offset.top

  # The above does not work for NetFront
  left = target.offsetLeft if left == 0
  top  = target.offsetTop  if top  == 0

  x = event.pageX - left
  y = event.pageY - top
  [x, y]

onDocumentReady = ->
  # TODO: display loading progress bar
#  $('img').load(->
#    #console.log(this)
#  )

  # Set JSG canvas height to full screen
  h = $(window).height()
  w = h * Stage.STANDARD_WIDTH / Stage.STANDARD_HEIGHT
  jsg.createCanvasAndContextForJSG(w, h)

  # Add JSG canvas to center of screen
  canvas = jsg.canvas
  canvas.style.position   = 'fixed'
  canvas.style.left       = '50%'
  canvas.style.marginLeft = "-#{w/2}px"
  document.body.appendChild(canvas)

  # Display splash screen while loading other resources
  jsg.loadImage('images/Default@2x.png', (img) ->
    jsg.context.drawImage(img, 0, 0, w, h)
  )

  # Wait after all resources have been loaded
  onWindowLoad = =>
    jsg.fireReady(w, h)

    jsg.canvas.onmousemove = (e) ->
      [x, y] = getEventElementXY(e)
      jsg.touch.fireMove(x, y)

    jsg.canvas.onmousedown = (e) ->
      [x, y] = getEventElementXY(e)
      jsg.touch.fireDown(x, y)

    jsg.canvas.onmouseup = (e) ->
      [x, y] = getEventElementXY(e)
      jsg.touch.fireUp(x, y)

    jsg.canvas.onclick = (e) ->
      [x, y] = getEventElementXY(e)
      jsg.touch.fireTap(x, y)

    fps = 40
    setInterval('jsg.fireTick()', 1000 / fps)

  $(window).load(onWindowLoad)

#-------------------------------------------------------------------------------

jsg.load('scripts/jsg/platform/android/sound.js')

$(document).ready(onDocumentReady)
