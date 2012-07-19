jsg.plugin.nakamap =
  open: (nakamapClientId) ->
    jsg.android.callJava('js.g.plugin.Nakamap', 'open', nakamapClientId)
