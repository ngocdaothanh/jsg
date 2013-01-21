jsg.tick = (listener) ->
  @tickListeners ?= []
  @tickListeners.push(listener)

jsg.untick = (listener) ->
  return if !@tickListeners?
  idx = @tickListeners.indexOf(listener)
  @tickListeners.splice(idx, 1) if idx != -1

# Called by platform
jsg.fireTick = ->
  # Date.now is not available on all browsers!
  # http://stackoverflow.com/questions/221294/how-do-you-get-a-timestamp-in-javascript
  t = (new Date()).getTime()

  if @lastTickTimestamp?
    dt = t - @lastTickTimestamp
    @lastTickTimestamp = t

    if @tickListeners?
      for listener in @tickListeners
        # Check in case @tickListeners is changed by untick while we are looping
        listener(dt, t) if listener?

    dt
  else
    @lastTickTimestamp = t
    0
