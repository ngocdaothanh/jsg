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
