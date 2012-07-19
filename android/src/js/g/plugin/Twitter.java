package js.g.plugin;

import js.g.JSG;
import js.g.JSGActivity;

import org.json.JSONObject;

import com.sugree.twitter.TwitterError;

import twitter4j.AsyncTwitter;
import twitter4j.AsyncTwitterFactory;
import twitter4j.Status;
import twitter4j.TwitterAdapter;
import twitter4j.TwitterException;
import twitter4j.TwitterListener;
import twitter4j.TwitterMethod;
import twitter4j.auth.AccessToken;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

class TwitterPrefs {
    private static final String PREF_TWITTER_ACCESS_TOKEN = "jsg/twitter/access_token";
    private static final String PREF_TWITTER_SECRET_TOKEN = "jsg/twitter/secret_token";

    private static final Activity          activity = JSGActivity.getInstance();
    private static final SharedPreferences prefs    = activity.getPreferences(Context.MODE_PRIVATE);

    public static String getAccessToken() {
        return prefs.getString(PREF_TWITTER_ACCESS_TOKEN, null);
    }

    public static String getSecretToken() {
        return prefs.getString(PREF_TWITTER_SECRET_TOKEN, null);
    }

    public static void setAccessToken(String accessToken) {
        Editor editor = prefs.edit();
        editor.putString(PREF_TWITTER_ACCESS_TOKEN, accessToken);
        editor.commit();
    }

    public static void setSecretToken(String secretToken) {
        Editor editor = prefs.edit();
        editor.putString(PREF_TWITTER_SECRET_TOKEN, secretToken);
        editor.commit();
    }

    public static void removeAll() {
        Editor editor = prefs.edit();
        editor.remove(PREF_TWITTER_ACCESS_TOKEN);
        editor.remove(PREF_TWITTER_SECRET_TOKEN);
        editor.commit();
    }
}

public class Twitter {
    // The factory instance is re-useable and thread safe
    private static final AsyncTwitterFactory twitter4jFactory = new AsyncTwitterFactory();

    private static final TwitterListener twitterShareListener = new TwitterAdapter() {
        @Override
        public void updatedStatus(Status status) {
            JSG.postJS("jsg.plugin.twitter.fireShareOK()");
        }

        @Override
        public void onException(TwitterException e, TwitterMethod method) {
            JSG.postJS("jsg.plugin.twitter.fireShareNG()");
        }
    };

    final static Activity activity = JSGActivity.getInstance();

    private static com.sugree.twitter.Twitter twitter;
    private static String consumerKey;
    private static String consumerSecret;

    public static String login(final String json) {
        activity.runOnUiThread(new Runnable() {
            public void run() {
                parseConsumerKeyAndSceret(json);
                if (consumerKey == null || consumerSecret == null) {
                    JSG.postJS("jsg.plugin.twitter.fireLoginNG()");
                    return;
                }

                int iconId = activity.getResources().getIdentifier("twitter_icon", "drawable", activity.getPackageName());
                if (iconId == 0) {
                    Log.e("Twitter.login", "Copy twitter_icon.png to res/drawable directory");
                    JSG.postJS("jsg.plugin.twitter.fireLoginNG()");
                    return;
                }

                twitter = new com.sugree.twitter.Twitter(iconId);

                // Setting null will cause runtime error
                String accessToken = TwitterPrefs.getAccessToken();
                String secretToken = TwitterPrefs.getSecretToken();
                if (accessToken != null) twitter.setAccessToken(accessToken);
                if (secretToken != null) twitter.setSecretToken(secretToken);

                if (twitter.isSessionValid()) {
                    JSG.postJS("jsg.plugin.twitter.fireLoginOK()");
                    return;
                }

                twitter.authorize(
                        activity,
                        new Handler(),
                        consumerKey,
                        consumerSecret,
                        new com.sugree.twitter.Twitter.DialogListener() {
                    public void onComplete(Bundle values) {
                        TwitterPrefs.setAccessToken(values.getString("access_token"));
                        TwitterPrefs.setSecretToken(values.getString("secret_token"));
                        JSG.postJS("jsg.plugin.twitter.fireLoginOK()");
                    }

                    public void onTwitterError(TwitterError e) {
                        JSG.postJS("jsg.plugin.twitter.fireLoginNG()");
                    }

                    public void onError(com.sugree.twitter.DialogError e) {
                        JSG.postJS("jsg.plugin.twitter.fireLoginNG()");
                    }

                    public void onCancel() {
                        JSG.postJS("jsg.plugin.twitter.fireLoginNG()");
                    }
                });
            }
        });
        return null;
    }

    public static String logout() {
        TwitterPrefs.removeAll();
        if (twitter != null) {
            try {
                twitter.logout(activity);
            } catch (Exception e) {
            }
        }
        return null;
    }

    public static String share(String message) {
        String at = TwitterPrefs.getAccessToken();
        String st = TwitterPrefs.getSecretToken();
        if (at == null || st == null || consumerKey == null || consumerSecret == null) {
            Log.e("Twitter.share", "Please call Twitter.login first");
            JSG.postJS("jsg.plugin.twitter.fireShareNG()");
        } else {
            AccessToken accessToken = new AccessToken(at, st);
            AsyncTwitter asyncTwitter = twitter4jFactory.getInstance();
            asyncTwitter.addListener(twitterShareListener);
            asyncTwitter.setOAuthConsumer(consumerKey, consumerSecret);
            asyncTwitter.setOAuthAccessToken(accessToken);
            asyncTwitter.updateStatus(message);
        }
        return null;
    }

    private static void parseConsumerKeyAndSceret(String json) {
        try {
            JSONObject obj = new JSONObject(json);
            consumerKey    = obj.getString("consumerKey");
            consumerSecret = obj.getString("consumerSecret");
        } catch (Exception e) {
            Log.e("Twitter.parseConsumerKeyAndSceret", "Could not parse JSON: " + json);
        }
    }
}
