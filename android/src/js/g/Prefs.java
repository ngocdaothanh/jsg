package js.g;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class Prefs {
    /** Default: null */
    public static synchronized String getString(String key) {
        SharedPreferences sp = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
        return sp.getString(key, null);
    }

    public static synchronized void setString(final String key, final String value) {
        // Committing preferences is usually slow on first time
        new Thread(new Runnable() {
            public void run() {
                SharedPreferences sp = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
                Editor e = sp.edit();
                e.putString(key, value);
                e.commit();
            }
        }).start();
    }

    //--------------------------------------------------------------------------

    /** Default: 0 */
    public static synchronized int getInt(String key) {
        SharedPreferences sp = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
        return sp.getInt(key, 0);
    }

    public static synchronized void setInt(final String key, final int value) {
        // Committing preferences is usually slow on first time
        new Thread(new Runnable() {
            public void run() {
                SharedPreferences sp = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
                Editor e = sp.edit();
                e.putInt(key, value);
                e.commit();
            }
        }).start();
    }
}
