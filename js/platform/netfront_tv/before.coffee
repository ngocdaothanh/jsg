isNetFront = (navigator.userAgent.indexOf('NetFront') >= 0)
if isNetFront
  document.write('<object id="appmgr" type="application/oipfApplicationManager" style="position: absolute; left: 0px; top: 0px; width: 0px; height: 0px;"></object>')

#-------------------------------------------------------------------------------

# http://whattheheadsaid.com/2010/10/a-safer-object-keys-compatibility-implementation
Object.keys = Object.keys || ((o) ->
  ret = []
  for k, v of o
    ret.push(k) if o.hasOwnProperty(k)
  ret
)

if !window.console?
  window.console =
    log: ->

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

  # NetFront 4.2 does not have context.textAlign (always "left")
  if isNetFront
    # NetFront 4.2 does not have measureText
    textWidth = fontSize * text.length
    x = switch alignx
      when Label.CENTER then (canvas.width - textWidth) / 2
      when Label.LEFT   then 0
      when Label.RIGHT  then canvas.width - textWidth
  else
    [context.textAlign, x] = switch alignx
      when Label.CENTER then ['center', canvas.width / 2]
      when Label.LEFT   then ['left',   0]
      when Label.RIGHT  then ['right',  canvas.width]

  # NetFront 4.2 does not have context.textBaseline (always "bottom")
  if isNetFront
    y = switch aligny
      when Label.CENTER then (canvas.height + fontSize) / 2
      when Label.TOP    then fontSize
      when Label.BOTTOM then canvas.height
  else
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

jsgFontFace = (fontFamily, src) ->

jsgLocale =
  getLanguage: ->
    'ja'

  getCountry: ->
    'jp'

  getVariant: ->
    ''
