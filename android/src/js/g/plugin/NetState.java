package js.g.plugin;

import js.g.JSGActivity;
import android.app.Activity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class NetState {
    /** For JS */
    public static String isOnline() {
        Activity            activity      = JSGActivity.getInstance();
        ConnectivityManager conMgr        =  (ConnectivityManager) activity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo         activeNetwork = conMgr.getActiveNetworkInfo();
        return (activeNetwork != null && activeNetwork.getState() == NetworkInfo.State.CONNECTED)? "" : null;
    }
}
