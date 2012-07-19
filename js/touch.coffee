class Touch
  constructor: ->
    @downListeners = []
    @upListeners   = []
    @moveListeners = []
    @tapListeners  = []

  down: (listener) ->
    @downListeners.push(listener)
    this

  up: (listener) ->
    @upListeners.push(listener)
    this

  move: (listener) ->
    @moveListeners.push(listener)
    this

  tap: (listener) ->
    @tapListeners.push(listener)
    this

  #-----------------------------------------------------------------------------

  fireDown: (x, y) ->
    for listener in @downListeners
      listener(x, y)
    null

  fireUp: (x, y) ->
    for listener in @upListeners
      listener(x, y)
    null

  fireMove: (x, y) ->
    for listener in @moveListeners
      listener(x, y)
    null

  fireTap: (x, y) ->
    for listener in @tapListeners
      listener(x, y)
    null

  #-----------------------------------------------------------------------------

  unlisten: (listener) ->
    @downListeners = _.without(@downListeners, listener)
    @upListeners   = _.without(@upListeners,   listener)
    @moveListeners = _.without(@moveListeners, listener)
    @tapListeners  = _.without(@tapListeners,  listener)

#-------------------------------------------------------------------------------

# Normally games should not use jsg.touch directly.
# Use Node#touch instead because it's easier and more effective.
# Events are not fired for a Node if its width/height is 0, it or its parent is
# invisible or inactive.
jsg.touch = new Touch
