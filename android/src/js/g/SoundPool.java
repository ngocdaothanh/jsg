package js.g;

import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.util.Log;

// http://developer.android.com/reference/android/media/SoundPool.html
public class SoundPool {
    private static final int MAX_SIMULTANEOUS_STREAMS = 5;

    private static android.media.SoundPool sp = new android.media.SoundPool(MAX_SIMULTANEOUS_STREAMS, AudioManager.STREAM_MUSIC, 0);

    public static String load(String src) {
        try {
            AssetFileDescriptor afd = JSGActivity.getInstance().getAssets().openFd(src);
            int ret = sp.load(afd, 1);
            return "" + ret;
        } catch (Exception e) {
            Log.e("SoundPool", "Could not load: " + src, e);
            return null;
        }
    }

    public static String play(String soundIDS) {
        int soundID = Integer.parseInt(soundIDS);
        playTest(soundID);
        int ret = sp.play(soundID, 1, 1, 0, 0, 1);
        return "" + ret;
    }

    public static String playLoop(String soundIDS) {
        int soundID = Integer.parseInt(soundIDS);
        playTest(soundID);
        int ret = sp.play(soundID, 1, 1, 0, -1, 1);
        return "" + ret;
    }

    public static String pause(String streamID) {
        sp.pause(Integer.parseInt(streamID));
        return null;
    }

    public static String stop(String streamID) {
        sp.stop(Integer.parseInt(streamID));
        return null;
    }

    // Wait for decoding to complete by try playing with volume 0.
    // http://blog.fujiu.jp/2011/06/android-soundpool.html
    private static void playTest(int soundID) {
        int streamID = 0;
        int count    = 0;
        do {
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
            }
            streamID = sp.play(soundID, 0, 0, 1, 0, 1);
            count++;
        } while (streamID == 0 && count < 1000);
    }
}
