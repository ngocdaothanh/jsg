package js.g.plugin;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import js.g.JSG;
import js.g.JSGActivity;
import js.g.plugin.billing.BillingService;
import js.g.plugin.billing.BillingService.RequestPurchase;
import js.g.plugin.billing.BillingService.RestoreTransactions;
import js.g.plugin.billing.Consts;
import js.g.plugin.billing.Consts.ResponseCode;
import js.g.plugin.billing.PurchaseObserver;
import js.g.plugin.billing.ResponseHandler;
import js.g.plugin.billing.Security;
import js.g.plugin.billing.Consts.PurchaseState;

/** See Dungeons billing example */
class JSGPurchaseObserver extends PurchaseObserver {
    private static final String PREFS_RESTORED = "jsg/billing/restored";

    private BillingService billingService;

    public JSGPurchaseObserver(BillingService billingService, Handler handler) {
        super(JSGActivity.getInstance(), handler);
        this.billingService = billingService;
    }

    @Override
    public void onBillingSupported(boolean supported, String type) {
        if (type == null || type.equals(Consts.ITEM_TYPE_INAPP)) {
            if (supported) {
                Log.i("Billing", "Restore purchased managed products on first run...");
                SharedPreferences prefs = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
                boolean restored = prefs.getBoolean(PREFS_RESTORED, false);
                if (!restored) billingService.restoreTransactions();
            }
        }
    }

    @Override
    public void onRestoreTransactionsResponse(RestoreTransactions request,
            ResponseCode responseCode) {
        if (responseCode == ResponseCode.RESULT_OK) {
            Log.i("Billing", "Restored purchased managed products");

            // Update PREFS_RESTORED
            SharedPreferences prefs = JSGActivity.getInstance().getPreferences(Context.MODE_PRIVATE);
            SharedPreferences.Editor edit = prefs.edit();
            edit.putBoolean(PREFS_RESTORED, true);
            edit.commit();
        } else {
            Log.e("Billing", "Could not restore purchased managed products");
        }
    }

    @Override
    public void onRequestPurchaseResponse(RequestPurchase request,
            ResponseCode responseCode) {
        if (responseCode == ResponseCode.RESULT_OK) {
            Log.i("Billing", "Purchase " + request.mProductId + " was successfully sent to server");
        } else if (responseCode == ResponseCode.RESULT_USER_CANCELED) {
            // User dismissed purchase dialog
            JSG.postJS("jsg.plugin.billing.firePurchased('" + request.mProductId + "', false)");
        } else {
            JSG.postJS("jsg.plugin.billing.firePurchased('" + request.mProductId + "', false)");
        }
    }

    @Override
    public void onPurchaseStateChange(PurchaseState purchaseState,
            String productId, long purchaseTime, String developerPayload) {
        Log.i("Billing", "onPurchaseStateChange, purchaseState: " + purchaseState + ", productId: " + productId);
        if (purchaseState == PurchaseState.PURCHASED)
            JSG.postJS("jsg.plugin.billing.firePurchased('" + productId + "', true)");
    }
}

public class Billing {
    private static BillingService billingService;

    public static String setPublicKey(final String base64EncodedPublicKey) {
        JSG.runOnUiThread(new Runnable() {
            public void run() {
                Security.base64EncodedPublicKey = base64EncodedPublicKey;

                billingService = new BillingService();
                billingService.setContext(JSGActivity.getInstance());

                // Avoid error:
                // Activity com.kayac.monatsu1.Monatsu1Activity has leaked ServiceConnection
                JSGActivity.getInstance().addOnDestroyListener(new Runnable() {
                    public void run() {
                        billingService.unbind();
                    }
                });

                // Check if billing is supported
                ResponseHandler.register(new JSGPurchaseObserver(billingService, new Handler()));
                if (!billingService.checkBillingSupported(Consts.ITEM_TYPE_INAPP)) {
                    Toast.makeText(JSGActivity.getInstance(), "Cannot connect to Play Store", Toast.LENGTH_LONG);
                }
            }
        });

        return null;
    }

    public static String purchase(final String productId) {
        JSG.runOnUiThread(new Runnable() {
            public void run() {
                billingService.requestPurchase(productId, Consts.ITEM_TYPE_INAPP, null);
            }
        });
        return null;
    }
}
