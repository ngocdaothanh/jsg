jsg.plugin.twitter =
  login: (consumerKey, consumerSecret) ->
    jsg.android.callJava(
      'js.g.plugin.Twitter',
      'login',
      {consumerKey: consumerKey, consumerSecret: consumerSecret}
    )

  logout: ->
    jsg.android.callJava('js.g.plugin.Twitter', 'logout')

  share: (message) ->
    jsg.android.callJava('js.g.plugin.Twitter', 'share', message)

  #-----------------------------------------------------------------------------

  loginOK: (listener) ->
    @loginOKs ?= []
    @loginOKs.push(listener)

  loginNG: (listener) ->
    @loginNGs ?= []
    @loginNGs.push(listener)

  shareOK: (listener) ->
    @shareOKs ?= []
    @shareOKs.push(listener)

  shareNG: (listener) ->
    @shareNGs ?= []
    @shareNGs.push(listener)

  fireLoginOK: ->
    return if !@loginOKs?
    for listener in @loginOKs
      listener()

  fireLoginNG: ->
    return if !@loginNGs?
    for listener in @loginNGs
      listener()

  fireShareOK: ->
    return if !@shareOKs?
    for listener in @shareOKs
      listener()

  fireShareNG: ->
    return if !@shareNGs?
    for listener in @shareNGs
      listener()
