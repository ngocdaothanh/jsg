jsg.plugin.netstate =
  isOnline: ->
    jsg.android.callJava('js.g.plugin.NetState', 'isOnline') != null
