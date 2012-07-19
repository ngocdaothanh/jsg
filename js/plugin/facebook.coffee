jsg.plugin.facebook =
  login: (appId) ->
    jsg.android.callJava('js.g.plugin.Facebook', 'login', appId)

  logout: ->
    jsg.android.callJava('js.g.plugin.Facebook', 'logout')

  # params:
  # {
  #   link:        'http://www.kayac.com'
  #   picture:     'http://www.kayac.com/img/common/logo.png'
  #   name:        'KAYAC Inc.'
  #   caption:     'Short caption'
  #   description: 'Long description'
  #   message:     'Some meaningful message'
  # }
  share: (params) ->
    jsg.android.callJava('js.g.plugin.Facebook', 'share', params)

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
