jsg.tick = (listener) ->
  @tickListeners = [] if !@tickListeners?
  @tickListeners.push(listener)

jsg.untick = (listener) ->
  return if !@tickListeners?
  idx = @tickListeners.indexOf(listener)
  @tickListeners.splice(idx, 1) if idx != -1

jsg.fireTick = ->
  t = Date.now()

  @lastTickTimestamp ?= t
  dt = t - @lastTickTimestamp
  @lastTickTimestamp = t

  if @tickListeners?
    for listener in @tickListeners
      # Check in case @tickListeners is changed by untick while we are looping
      listener(dt, t) if listener?

  [dt, t]
