# See com.kayac.monatsu1.AdMob
jsg.plugin.admob =
  # Returns 0 if the banner height has not been calculated,
  # or calculated but the banner has been hidden immediately
  # at first time.
  getHeight: ->
    height = jsg.prefs.getInt('jsg/admob/height')
    return 0 if height == 0

    secondTimeOn = jsg.prefs.getInt('jsg/admob/secondTimeOn')
    if secondTimeOn == 1 then height else 0
