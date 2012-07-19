class Timer
  constructor: ->
    @funs       = []
    @timestamps = []
    jsg.tick(@onTick)

  # Returns id for used at clearTimeout
  setTimeout: (fun, delay) =>
    @funs.push(fun)
    @timestamps.push(Date.now() + delay)
    fun

  clearTimeout: (id) =>
    i = @funs.indexOf(id)
    if i > -1
      @funs.splice(i, 1)
      @timestamps.splice(i, 1)

  onTick: (dt, t) =>
    i = 0
    while i < @timestamps.length
      c = @timestamps[i]
      if c <= t
        # @funs and @timestamps may be changed by fun
        fun = @funs[i]
        @funs.splice(i, 1)
        @timestamps.splice(i, 1)
        fun()
      else
        i++

#-------------------------------------------------------------------------------

jsg.timer    = new Timer
setTimeout   = jsg.timer.setTimeout
clearTimeout = jsg.timer.clearTimeout
