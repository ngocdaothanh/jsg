package js.g;

import java.util.ArrayList;

import android.app.Activity;
import android.media.AudioManager;
import android.os.Bundle;

public class JSGActivity extends Activity {
    private static JSGActivity instance = null;

    public static JSGActivity getInstance() {
        return instance;
    }

    // Called by JS
    public static String quit() {
        // http://stackoverflow.com/questions/2041891/sending-activity-to-background-with-out-finishing
        instance.moveTaskToBack(true);
        return null;
    }

    //--------------------------------------------------------------------------

    private boolean paused;

    private ArrayList<Runnable> onResumeListeners  = new ArrayList<Runnable>();
    private ArrayList<Runnable> onPauseListeners   = new ArrayList<Runnable>();
    private ArrayList<Runnable> onDestroyListeners = new ArrayList<Runnable>();

    public boolean isPaused() {
        return paused;
    }

    public void addOnResumeListener(Runnable runnable) {
        synchronized (onResumeListeners) {
            onResumeListeners.add(runnable);
        }
    }

    public void addOnPauseListener(Runnable runnable) {
        synchronized (onPauseListeners) {
            onPauseListeners.add(runnable);
        }
    }

    /** For services etc. to know when to release their resources. */
    public void addOnDestroyListener(Runnable runnable) {
        synchronized (onDestroyListeners) {
            onDestroyListeners.add(runnable);
        }
    }

    //--------------------------------------------------------------------------

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        instance = this;

        // http://stackoverflow.com/questions/2539264/volume-control-in-android-application
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
    }

    @Override
    protected void onResume() {
        paused = false;

        super.onResume();

        synchronized (onResumeListeners) {
            for (Runnable runnable: onResumeListeners) {
                runnable.run();
            }
        }
        // Skip the first resume (run after onCreate)
        if (JSG.isReady()) JSG.postJS("jsg.fireResume()");
    }

    @Override
    protected void onPause() {
        paused = true;

        synchronized (onPauseListeners) {
            for (Runnable runnable: onPauseListeners) {
                runnable.run();
            }
        }

        JSG.postJS("jsg.firePause()");
        super.onPause();
    }

    @Override
    public void onDestroy() {
        synchronized (onDestroyListeners) {
            for (Runnable runnable: onDestroyListeners) {
                runnable.run();
            }
        }

        super.onDestroy();
        instance = null;
    }

    @Override
    public void onBackPressed() {
        if (JSG.isReady()) JSG.postJS("jsg.android.fireBackButton()");
    }
}
