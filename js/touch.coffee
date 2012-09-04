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

  onFrame: (touchActions, touchXs, touchYs) ->
    if touchActions?
      length = touchActions.length
      for i in [0...length]
        action = touchActions[i]
        x      = touchXs[i]
        y      = touchYs[i]
        if action == Touch.DOWN
          @fireDown(x, y)
        else if action == Touch.MOVE
          @fireMove(x, y)
        else if action == Touch.UP
          @fireUp(x, y)
        else
          @fireTap(x, y)
    null

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

Touch.DOWN = 0
Touch.MOVE = 1
Touch.UP   = 2
Touch.TAP  = 3

jsg.touch = new Touch
