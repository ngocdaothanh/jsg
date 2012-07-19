package js.g;

public class Locale {
    public static String getLanguage() {
        return JSGActivity.getInstance().getResources().getConfiguration().locale.getLanguage();
    }

    public static String getCountry() {
        return JSGActivity.getInstance().getResources().getConfiguration().locale.getCountry();
    }

    public static String getVariant() {
        return JSGActivity.getInstance().getResources().getConfiguration().locale.getVariant();
    }
}
