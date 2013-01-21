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

# This style does not work on NetFront:
# documentReady = if isNetFront then $(window).load else $(document).ready
# documentReady(-> ...)

onDocumentReady = ->
  if isNetFront
    appManager = document.getElementById("appmgr")
    app        = appManager.getOwnerApplication(document)
    app.show()
    app.activateInput(true)

    keyset = app.privateData.keyset
    keysetValue = keyset.RED | keyset.GREEN | keyset.YELLOW | keyset.BLUE | keyset.NAVIGATION | keyset.VCR | keyset.SCROLL | keyset.INFO | keyset.NUMERIC | keyset.ALPHA | keyset.OTHER
    otherKeys = [
      409,	# 0x199 電源
      500,	# 0x1F4 スリープ
      427,	# 0x1AB チャンネル
      428,	# 0x1AC チャンネル
      503,	# 0x1F7 地デジ
      504,	# 0x1F8 BS
      505,	# 0x1F9 CATV
      506,	# 0x1FA ３桁入力CH
      507,	# 0x1FB VOD
      508,	# 0x1FC ブラウザ
      462,	# 0x1CE メニュー
      509,	# 0x1FD 番組表
      510,	# 0x1FE ｄ
      461,	# 0x1CD 戻る
      511,	# 0x1FF お好み
      512,	# 0x200 検索
      513,	# 0x201 ＣＨ一覧
      514,	# 0x202 ＣＨリターン
      515,	# 0x203 映像
      516,	# 0x204 音声
      460,	# 0x1CC 字幕
      416,	# 0x1A0 録画
      517,	# 0x205 録画一覧
      518,	# 0x206 予約一覧
      519 	# 0x207 再生パネル
    ]
    try
      keyset.setValue(keysetValue, otherKeys)
    catch e
      alert('Exception: ' + e.message)

  # TODO: display loading progress bar
#  $('img').load(->
#    #console.log(this)
#  )

  # Set JSG canvas height to full screen
  h =
    if isNetFront
      720
    else
      ret = $(window).height()
      # The above line does not work for NetFront
      if ret == 0 then window.innerHeight else ret
  w = h * Stage.STANDARD_WIDTH / Stage.STANDARD_HEIGHT
  jsg.createCanvasAndContextForJSG(w, h)

  # Add JSG canvas to center of screen
  canvas = jsg.canvas
  canvas.style.position   = 'fixed'
  canvas.style.left       = '50%'
  canvas.style.marginLeft = "-#{w/2}px"
  document.body.appendChild(canvas)

  # Display splash screen while loading other resources
  if !isNetFront
    jsg.loadImage('images/Default@2x.png', (event) ->
      splashImage = event.target
      jsg.context.drawImage(splashImage, 0, 0, w, h)
    )

  # Load font
  # http://stackoverflow.com/questions/2756575/drawing-text-to-canvas-with-font-face-does-not-work-at-the-first-time
  if !isNetFront
    $('body').append('<div id="loadfont" style="font-family: \'fonts/GD-DOTFONT-DQ-TTF_008.ttf\'">.</div>')

  # Wait after all resources have been loaded
  onWindowLoad = =>
    $('#loadfont').remove() if !isNetFront

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

    fps = if isNetFront then 10 else 60
    setInterval('jsg.fireTick()', 1000 / fps)

  if isNetFront then onWindowLoad() else $(window).load(onWindowLoad)

if isNetFront then $(window).load(onDocumentReady) else $(document).ready(onDocumentReady)
