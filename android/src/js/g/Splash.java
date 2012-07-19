package js.g;

import java.io.InputStream;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.widget.ImageView;

public class Splash extends ImageView {
    public Splash(String src) {
        super(JSGActivity.getInstance());

        try {
            AssetManager assetManager = getContext().getAssets();
            InputStream  is           = assetManager.open(src);
            Bitmap       bitmap       = BitmapFactory.decodeStream(is);

            setImageBitmap(bitmap);
            setScaleType(ScaleType.FIT_XY);
        } catch (Exception e) {
            Log.e("Splash", "Could not load image: " + src, e);
        }
    }
}
