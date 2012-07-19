package js.g;

import java.util.Map;

import android.app.AlertDialog;
import android.content.DialogInterface;

import com.fasterxml.jackson.databind.ObjectMapper;

public class Dialog {
    public static String alert(final String message) {
        JSG.runOnUiThread(new Runnable() {
            public void run() {
                new AlertDialog.Builder(JSGActivity.getInstance())
                .setPositiveButton("OK",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dlg, int which) {
                        }
                    }
                ).setMessage(message)
                .show();
            }
        });
        return null;
    }

    private static ObjectMapper mapper = new ObjectMapper();

    public static String confirm(final String json) {
        JSG.runOnUiThread(new Runnable() {
            @SuppressWarnings("unchecked")
            public void run() {
                Map<String, Object> map = null;
                try {
                    map = mapper.readValue(json, Map.class);
                } catch (Exception e) {

                }
                String message = (String) map.get("message");
                final int callbackId = (Integer) map.get("callbackId");

                new AlertDialog.Builder(JSGActivity.getInstance())
                .setMessage(message)
                .setPositiveButton("OK",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dlg, int which) {
                            JSG.postJS(String.format("jsg.callback.invoke(%d, true)", callbackId));
                        }
                    }
                )
                .setNegativeButton("Cancel",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dlg, int which) {
                            JSG.postJS(String.format("jsg.callback.invoke(%d, false)", callbackId));
                        }
                    }
                )
                .show();
            }
        });
        return null;
    }
}
