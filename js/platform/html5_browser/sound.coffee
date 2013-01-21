# You need to provide both .mp3 and .ogg versions of each sound.
# Depending on browser, either myfile.mp3 or myfile.ogg will be loaded:
# sound = new Sound('myfile')
# sound.play()
class Sound
  constructor: (base) ->
    src = Sound.mp3OrOgg(base)
    @audio = new Audio(src)

  play: ->
    @audio.play() if !jsg.paused

  # Can be passed as callback to jsg.resume
  playLoop: =>
    return if jsg.paused
    if false #@audio.playLoop?
      @audio.playLoop()
    else
      @audio.loop = true
      @audio.play()

  stop: ->
    #if @audio.stop? then @audio.stop() else @audio.pause()
    @audio.pause()

  pause: ->
    @audio.pause()

Sound.mp3OrOgg(base) = ->
  if !@cachedCanPlayOgg?
    audio = new Audio('')
    @cachedCanPlayOgg = ('' != audio.canPlayType('audio/ogg'))

  ext = if @cachedCanPlayOgg then 'ogg' else 'mp3'
  base + '.' + ext
