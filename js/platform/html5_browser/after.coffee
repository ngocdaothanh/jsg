jsg.platform = 'HTML5'

jsg.load('scripts/jsg/platform/android/sound.js')

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

    fps = 60
    setInterval('jsg.fireTick()', 1000 / fps)

  $(window).load(onWindowLoad)

$(document).ready(onDocumentReady)
