package js.g.plugin;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.MalformedURLException;
import java.util.Iterator;

import org.json.JSONObject;

import js.g.JSG;
import js.g.JSGActivity;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.util.Log;

import com.facebook.android.AsyncFacebookRunner;
import com.facebook.android.DialogError;
import com.facebook.android.Facebook.DialogListener;
import com.facebook.android.FacebookError;

class FacebookPrefs {
    private static final String PREF_JSG_FACEBOOK_ACCESS_TOKEN = "jsg/facebook/access_token";
    private static final String PREF_JSG_FACEBOOK_EXPIRES      = "jsg/facebook/expires";

    private static final Activity          activity = JSGActivity.getInstance();
    private static final SharedPreferences prefs    = activity.getPreferences(Context.MODE_PRIVATE);

    public static String getAccessToken() {
        return prefs.getString(PREF_JSG_FACEBOOK_ACCESS_TOKEN, null);
    }

    public static long getAccessExpires() {
        return prefs.getLong(PREF_JSG_FACEBOOK_EXPIRES, 0);
    }

    public static void setAccessToken(String accessToken) {
        Editor editor = prefs.edit();
        editor.putString(PREF_JSG_FACEBOOK_ACCESS_TOKEN, accessToken);
        editor.commit();
    }

    public static void setAccessExpires(long accessExpires) {
        Editor editor = prefs.edit();
        editor.putLong(PREF_JSG_FACEBOOK_EXPIRES, accessExpires);
        editor.commit();
    }

    public static void removeAll() {
        Editor editor = prefs.edit();
        editor.remove(PREF_JSG_FACEBOOK_ACCESS_TOKEN);
        editor.remove(PREF_JSG_FACEBOOK_EXPIRES);
        editor.commit();
    }
}

public class Facebook {
    private static final Activity activity = JSGActivity.getInstance();
    private static com.facebook.android.Facebook facebook;

    public static String login(final String appId) {
        facebook = new com.facebook.android.Facebook(appId);
        facebook.setAccessToken(FacebookPrefs.getAccessToken());
        facebook.setAccessExpires(FacebookPrefs.getAccessExpires());

        if (facebook.isSessionValid()) {
            JSG.postJS("jsg.plugin.facebook.fireLoginOK()");
            return null;
        }

        activity.runOnUiThread(new Runnable() {
            public void run() {
                Bundle params = new Bundle();
                params.putString("scope", "publish_stream");
                facebook.dialog(activity, "oauth", params, new DialogListener() {
                    public void onComplete(Bundle values) {
                        // For facebook.dialog()
                        String token   = values.getString("access_token");
                        long   expires = Long.valueOf(values.getString("expires_in"));

                        facebook.setAccessToken(token);
                        facebook.setAccessExpires(expires);

                        FacebookPrefs.setAccessToken(token);
                        FacebookPrefs.setAccessExpires(expires);

                        JSG.postJS("jsg.plugin.facebook.fireLoginOK()");
                    }

                    public void onFacebookError(FacebookError e) {
                        JSG.postJS("jsg.plugin.facebook.fireLoginNG()");
                    }

                    public void onError(DialogError e) {
                        JSG.postJS("jsg.plugin.facebook.fireLoginNG()");
                    }

                    public void onCancel() {
                        JSG.postJS("jsg.plugin.facebook.fireLoginNG()");
                    }
                });
            }
        });

        return null;
    }

    public static String logout() {
        FacebookPrefs.removeAll();
        if (facebook != null) {
            AsyncFacebookRunner afacebook = new AsyncFacebookRunner(facebook);
            afacebook.logout(activity, new AsyncFacebookRunner.RequestListener() {
                public void onMalformedURLException(MalformedURLException e, Object state) {
                }

                public void onIOException(IOException e, Object state) {
                }

                public void onFileNotFoundException(FileNotFoundException e, Object state) {
                }

                public void onFacebookError(FacebookError e, Object state) {
                }

                public void onComplete(String response, Object state) {
                }
            });
        }
        return null;
    }

    public static String share(final String jsonParams) {
        Bundle params = parseShareParams(jsonParams);
        if (params == null) {
            JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            return null;
        }

        if (facebook == null) {
            Log.e("Facebook.share", "Please call Facebook.login first");
            JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            return null;
        }

        params.putString("access_token", facebook.getAccessToken());
        AsyncFacebookRunner afacebook = new AsyncFacebookRunner(facebook);
        afacebook.request("me/feed", params, "POST", new AsyncFacebookRunner.RequestListener() {
            public void onMalformedURLException(MalformedURLException e, Object state) {
                JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            }

            public void onIOException(IOException e, Object state) {
                JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            }

            public void onFileNotFoundException(FileNotFoundException e, Object state) {
                JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            }

            public void onFacebookError(FacebookError e, Object state) {
                JSG.postJS("jsg.plugin.facebook.fireShareNG()");
            }

            public void onComplete(String response, Object state) {
                JSG.postJS("jsg.plugin.facebook.fireShareOK()");
            }
        }, null);

        return null;
    }

    private static Bundle parseShareParams(String jsonParams) {
        try {
            JSONObject objParams = new JSONObject(jsonParams);
            Bundle params = new Bundle();
            @SuppressWarnings("unchecked")
            Iterator<String> it = objParams.keys();
            while (it.hasNext()) {
                String key   = it.next();
                String value = null;
                value = objParams.getString(key);
                params.putString(key, value);
            }
            return params;
        } catch (Exception e) {
            Log.e("Facebook.parseShareParams", "Could not parse JSON: " + jsonParams);
            return null;
        }
    }
}
