# Used mostly for passing reference from JS to native platform (e.g. Java).
class IdGenerator
  constructor: ->
    @_next = 0

  next: ->
    ret = @_next
    if @_next == IdGenerator.MAX_SIGNED_INT_32
      @_next = 0
    else
      @_next++
    ret

IdGenerator.MAX_SIGNED_INT_32 = Math.pow(2, 31) - 1
