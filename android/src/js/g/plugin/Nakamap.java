package js.g.plugin;

import js.g.JSG;
import js.g.JSGActivity;

public class Nakamap {
    private static String lastId = null;

    public static String open(final String id) {
        JSG.runOnUiThread(new Runnable() {
            public void run() {
                if (id != lastId) {
                    com.kayac.nakamap.sdk.Nakamap.setup(
                            JSGActivity.getInstance(),
                            id,
                            "my_nakamap_account");
                    lastId = id;
                }
                com.kayac.nakamap.sdk.Nakamap.showChatView();
            }
        });
        return null;
    }
}
