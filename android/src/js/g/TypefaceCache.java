package js.g;

import java.util.HashMap;
import java.util.Map;

import android.graphics.Typeface;

public class TypefaceCache {
    private static Map<String, Typeface> cache = new HashMap<String, Typeface>();

    /** Set type face for paint, now it support .ttf file. */
    public static synchronized Typeface get(String src) {
        if (cache.containsKey(src)) return cache.get(src);

        Typeface ret;
        if (src.endsWith(".ttf")) {
            ret = Typeface.createFromAsset(JSGActivity.getInstance().getAssets(), src);
        } else {
            ret = Typeface.create(src, Typeface.NORMAL);
        }
        cache.put(src, ret);
        return ret;
    }
}
