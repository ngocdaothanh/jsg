jsgCreateCanvasAndContext = (width, height) ->
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

jsgAlert = (message) ->
  jsg.android.callJava('js.g.Dialog', 'alert', message)

# Implement window.confirm feature
# callback: (boolean) ->
jsgConfirm = (message, callback) ->
  callbackId = @callback.register(callback)
  jsg.android.callJava('js.g.Dialog', 'confirm', callbackId + ',' + message)

jsgQuit = ->
  jsg.android.callJava('js.g.JSGActivity', 'quit')

jsgLocale =
  getLanguage: ->
    jsg.android.callJava('js.g.Locale', 'getLanguage')

  getCountry: ->
    jsg.android.callJava('js.g.Locale', 'getCountry')

  getVariant: ->
    jsg.android.callJava('js.g.Locale', 'getVariant')
