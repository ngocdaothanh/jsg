package js.g;

import java.util.ArrayList;
import java.util.List;

import android.content.res.AssetFileDescriptor;
import android.util.Log;

/*
TODO: Investigate

Uri uri = Uri.parse("android.resource://YOUR_PACKAGENAME/" + resources);
mPlayer.setDataSource(getApplicationContext(),uri);
*/
public class MediaPlayer {
    private static List<MediaPlayer> all = new ArrayList<MediaPlayer>();

    public static String pauseAll(String ingored) {
        for (MediaPlayer a: all) {
            a.pause();
        }
        return null;
    }

    //--------------------------------------------------------------------------

    private android.media.MediaPlayer player = null;

    public MediaPlayer(String src) {
        try {
            AssetFileDescriptor afd = JSGActivity.getInstance().getAssets().openFd(src);
            player = new android.media.MediaPlayer();
            player.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            player.prepare();
            all.add(this);
        } catch (Exception e) {
            Log.e("Audio", "Could not load: " + src, e);
            player = null;
        }
    }

    public void play() {
        if (player != null) {
            player.setLooping(false);
            player.start();
        }
    }

    public void playLoop() {
        if (player != null) {
            player.setLooping(true);
            player.start();
        }
    }

    public void pause() {
        if (player != null && player.isPlaying()) player.pause();
    }

    public void stop() {
        if (player != null && player.isPlaying()) player.stop();
    }
}
