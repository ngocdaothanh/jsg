package js.g.plugin;

import android.app.Activity;
import android.view.Gravity;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnPreDrawListener;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import com.google.ads.Ad;
import com.google.ads.AdListener;
import com.google.ads.AdRequest;
import com.google.ads.AdSize;
import com.google.ads.AdView;
import com.google.ads.AdRequest.ErrorCode;

import js.g.JSGActivity;
import js.g.Prefs;

public class AdMob {
    private static final String PREFS_HEIGHT         = "jsg/admob/height";
    private static final String PREFS_SECOND_TIME_ON = "jsg/admob/secondTimeOn";

    private static AdView adView;

    public static void addToBottom(String admobId) {
        Activity activity = JSGActivity.getInstance();

        adView = new AdView(activity, AdSize.SMART_BANNER, admobId);
        adView.loadAd(new AdRequest());

        FrameLayout.LayoutParams bottom = new FrameLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT);
        bottom.gravity = Gravity.BOTTOM;
        activity.addContentView(adView, bottom);
        saveHeightToPrefsOnFirstReceiveAd();
    }

    public static void addToBottomIfOnline(String admobId) {
        if (NetState.isOnline() != null) addToBottom(admobId);
    }

    public static int getHeight() {
        return Prefs.getInt(PREFS_HEIGHT);
    }

    //--------------------------------------------------------------------------

    private static void saveHeightToPrefsOnFirstReceiveAd() {
        int height = Prefs.getInt(PREFS_HEIGHT);
        if (height != 0) {
            int secondTimeOn = Prefs.getInt(PREFS_SECOND_TIME_ON);
            if (secondTimeOn == 0) Prefs.setInt(PREFS_SECOND_TIME_ON, 1);
            return;
        }

        adView.setAdListener(new AdListener() {
            OnPreDrawListener opdl;

            public void onReceiveAd(Ad arg0) {
                // onReceiveAd is called every time a new ad is received.
                // Skip if height has already been saved.
                int height = Prefs.getInt(PREFS_HEIGHT);
                if (height != 0) return;

                final ViewTreeObserver obs = adView.getViewTreeObserver();
                opdl = new OnPreDrawListener() {
                    public boolean onPreDraw () {
                        obs.removeOnPreDrawListener(opdl);

                        int height = adView.getHeight();
                        Prefs.setInt(PREFS_HEIGHT, height);

                        // The first run is only for getting the ad banner height.
                        // Ads will be displayed after app restart.
                        // Force app exit on pause so that the needed restart
                        // happens as soon as possible.
                        JSGActivity.getInstance().addOnPauseListener(new Runnable() {
                            public void run() {
                                if (!JSGActivity.getInstance().isActivityChangeInsideSameApp())
                                    System.exit(0);
                            }
                        });

                        // http://stackoverflow.com/questions/3080577/removing-a-view-from-an-activity
                        ViewGroup vg = (ViewGroup) (adView.getParent());
                        vg.removeView(adView);

                        adView.destroy();
                        adView = null;

                        return true;
                   }
                };
                obs.addOnPreDrawListener(opdl);
            }

            public void onPresentScreen(Ad arg0) {
            }

            public void onLeaveApplication(Ad arg0) {
            }

            public void onFailedToReceiveAd(Ad arg0, ErrorCode arg1) {
            }

            public void onDismissScreen(Ad arg0) {
            }
        });
    }
}
