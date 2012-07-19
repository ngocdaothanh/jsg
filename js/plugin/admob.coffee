# See com.kayac.monatsu1.AdMob
jsg.plugin.admob =
  # Returns null if none
  getHeightFromPrefs: ->
    heightString = jsg.prefs.getString('jsg/admob/height')
    if heightString? then parseInt(heightString) else null
