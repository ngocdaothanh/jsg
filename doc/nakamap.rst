Nakamap plugin
==============

See http://developer.nakamap.com/documents

AndroidManifest.xml

::

  ...

  <uses-permission android:name="android.permission.INTERNET" />
  <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE"/>
  <uses-permission android:name="android.permission.ACCESS_WIFI_STATE"/>
  <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

  ...

  <activity
      android:name="com.kayac.nakamap.sdk.activity.NakamapActivity"
      android:screenOrientation="portrait"
      android:configChanges="keyboard|keyboardHidden|orientation"
      android:theme="@android:style/Theme.Translucent.NoTitleBar">
      <intent-filter>
          <action android:name="android.intent.action.VIEW"/>
          <category android:name="android.intent.category.DEFAULT" />
          <category android:name="android.intent.category.BROWSABLE" />
          <data android:scheme="nakamapapp-NAKAMAP_CLIENT_ID"/>
      </intent-filter>
  </activity>

  ...

Open Nakamap chat:

::

  jsg.load('scripts/jsg/plugin/nakamap.js')
  nakamapClientId = '...'
  jsg.plugin.nakamap.open(nakamapClientId)
