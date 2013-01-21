jsg.load('scripts/jsg/platform/android/id_generator.js')
jsg.load('scripts/jsg/platform/android/callback.js')
jsg.load('scripts/jsg/platform/android/sound.js')

setTimeout    = jsg.setTimeout
clearTimeout  = jsg.clearTimeout
setInterval   = jsg.setInterval
clearInterval = jsg.clearInterval

# App global variables should be attached to "window" object
window = {}
