jsg.platform = 'Android'

jsg.android =
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

  # Called by Android
  fireBackButton: ->
    if @backButtonListeners?
      for listener in @backButtonListeners
        listener()
    null

jsg.load('scripts/jsg/platform/android/id_generator.js')
jsg.load('scripts/jsg/platform/android/callback.js')
jsg.load('scripts/jsg/platform/android/sound.js')

setTimeout    = jsg.setTimeout
clearTimeout  = jsg.clearTimeout
setInterval   = jsg.setInterval
clearInterval = jsg.clearInterval

# Some JS code use these names (ex: EaselJS)
HTMLCanvasElement = Canvas
HTMLImageElement  = Image
