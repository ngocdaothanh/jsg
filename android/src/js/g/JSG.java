package js.g;

import java.io.InputStream;
import java.util.concurrent.atomic.AtomicBoolean;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Handler;
import android.provider.MediaStore;

public class JSG {
    static {
        System.loadLibrary("jsg");
    }

    //--------------------------------------------------------------------------

    // Made public so that other Java code (may be written by JSG users) can use
    public static Handler mainThreadHandler = new Handler();

    private static native void nativeInit(int stageWidth, int stageHeight, String mainScript);

    private static AtomicBoolean ready = new AtomicBoolean(false);
    public static boolean isReady() { return ready.get(); }

    /** Must be run on the game thread */
    public static void init(final String mainScript, final int stageWidth, final int stageHeight) {
        nativeInit(stageWidth, stageHeight, mainScript);
        ready.set(true);
    }

    // For Java ----------------------------------------------------------------

    /** Run JS right away. The caller must be sure that it is running on the game thread. */
    public static native void runJS(String js);

    /** Schedule the JS to be run on the game thread as soon as possible. */
    public static void postJS(final String js) {
        Stage.getInstance().queueEvent(new Runnable() {
            public void run() {
                JSG.runJS(js);
            }
        });
    }

    public static void runOnUiThread(Runnable runnable) {
        //JSGActivity.getInstance().runOnUiThread(runnable);
        mainThreadHandler.post(runnable);
    }

    // For native --------------------------------------------------------------

    public static byte[] loadAsset(String fileName) {
        try {
            AssetManager assetManager = JSGActivity.getInstance().getAssets();
            InputStream is = assetManager.open(fileName);
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();
            return buffer;
        } catch (Exception e) {
            return null;
        }
    }

    public static void saveBitmapToSystemGallery(int[] argb, int stride, int width, int height, String title, String description) {
        Bitmap bm = Bitmap.createBitmap(argb, 0, stride, width, height, Bitmap.Config.ARGB_8888);
        MediaStore.Images.Media.insertImage(JSGActivity.getInstance().getContentResolver(), bm, title, description);
    }
}
