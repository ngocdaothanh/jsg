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
    idx = @downListeners.indexOf(listener)
    @downListeners.splice(idx, 1) if idx != -1

    idx = @upListeners.indexOf(listener)
    @upListeners.splice(idx, 1) if idx != -1

    idx = @moveListeners.indexOf(listener)
    @moveListeners.splice(idx, 1) if idx != -1

    idx = @tapListeners.indexOf(listener)
    @tapListeners.splice(idx, 1) if idx != -1

#-------------------------------------------------------------------------------

jsg.touch = new Touch
