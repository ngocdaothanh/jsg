# SoundPool is not applicable to playing long sounds.
class SoundPool
  # Returns soundId.
  load: (src) ->
    jsg.android.callJava('js.g.SoundPool', 'load', src)

  # Returns streamID used for stop or pause.
  play: (soundId) ->
    jsg.android.callJava('js.g.SoundPool', 'play', soundId)

  # Returns streamID used for stop or pause.
  playLoop: (soundId) ->
    jsg.android.callJava('js.g.SoundPool', 'playLoop', soundId)

  stop: (streamID) ->
    jsg.android.callJava('js.g.SoundPool', 'stop', streamID)

  pause: (streamID) ->
    jsg.android.callJava('js.g.SoundPool', 'pause', streamID)

#-------------------------------------------------------------------------------

jsg.soundPool = new SoundPool

#-------------------------------------------------------------------------------

class Sound
  constructor: (@src) ->
    @id = jsg.soundPool.load(src)

  play: ->
    if @id
      @streamId = jsg.soundPool.play(@id)
    else
      console.log "Could not load sound: #{@src}"

  playLoop: ->
    if @id
      @streamId = jsg.soundPool.playLoop(@id)
    else
      console.log "Could not load sound: #{@src}"

  stop: ->
    jsg.soundPool.stop(@streamId) if @streamId?

  pause: ->
    jsg.soundPool.pause(@streamId) if @streamId?
