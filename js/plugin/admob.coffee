# See com.kayac.monatsu1.AdMob
jsg.plugin.admob =
  # Returns null if none
  getHeightFromPrefs: ->
    jsg.prefs.getInt('jsg/admob/height')
