#jsg.setTimeout    = (f, a) -> window.setTimeout(f, a)
#jsg.clearTimeout  = (id)   -> window.clearTimeout(id)
#jsg.setInterval   = (f, a) -> window.setInterval(f, a)
#jsg.clearInterval = (id)   -> window.clearInterval(id)

# Creating many timers will make the system slow.
# Instead of using setTimeout and setInterval directly,
# use jsg.setTimeout and jsg.setInterval. These only
# share one timer.
class TickBasedTimer
  constructor: (@fun, @after) ->
    @acc = 0

    # true:  this instance is used for setTimeout
    # false: this instance is used for setInterval
    @isSetTimeout = true

  onTick: (dt, t) =>
    @acc += dt
    if @acc >= @after
      @fun()
      if @isSetTimeout
        jsg.untick(@onTick)
      else
        @acc = @acc % @after

  setTimeout: ->
    jsg.tick(@onTick)

  clearTimeout: ->
    jsg.untick(@onTick)

  setInterval: ->
    @isSetTimeout = false
    jsg.tick(@onTick)

  clearInterval: ->
    jsg.untick(@onTick)

# Returns a TickBasedTimer
jsg.setTimeout = (fun, after) ->
  id = new TickBasedTimer(fun, after)
  id.setTimeout()
  id

# id is a TickBasedTimer
jsg.clearTimeout = (id) ->
  id.clearTimeout()
  id = null

# Returns a TickBasedTimer
jsg.setInterval = (fun, after) ->
  id = new TickBasedTimer(fun, after)
  id.setInterval()
  id

# id is a TickBasedTimer
jsg.clearInterval = (id) ->
  id.clearTimeout()
  id = null
