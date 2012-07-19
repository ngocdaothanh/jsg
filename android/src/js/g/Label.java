package js.g;

import java.util.LinkedList;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Paint.FontMetricsInt;

/** JNI calls this utility to get bitmap of the label */
public class Label {
    private static final int CENTER = 0;

    private static final int LEFT  = 1;
    private static final int RIGHT = 2;

    private static final int TOP    = 1;
    private static final int BOTTOM = 2;

    /**
     * @param width can be 0
     * @param height can be 0
     */
    public static Bitmap createTextBitmap(
            int width, int height,
            String fontName, int fontSize,
            String text, String textColor, int alignx, int aligny) {

        text = refactorString(text);
        Paint paint = newPaint(fontName, fontSize, textColor, alignx);

        TextProperty textProperty = computeTextProperty(text, paint, width, height);
        int bitmapTotalHeight = (height == 0 ? textProperty.totalHeight : height);

        // Draw text to bitmap
        Bitmap bitmap = Bitmap.createBitmap(
                textProperty.maxWidth,
                bitmapTotalHeight,
                Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);

        // Draw string
        FontMetricsInt fm = paint.getFontMetricsInt();
        int x = 0;
        int y = 0;
        if (height == 0 || aligny == TOP) {
            y = -fm.top;
        } else {
            if (aligny == CENTER)
                y = -fm.top + (height - textProperty.totalHeight)/2;
            else if (aligny == BOTTOM)
                y = -fm.top + height - textProperty.totalHeight;
        }
        String[] lines = textProperty.lines;
        for (String line : lines) {
            x = computeX(paint, line, textProperty.maxWidth, alignx);
            canvas.drawText(line, x, y, paint);
            y += textProperty.heightPerLine;
        }

        return bitmap;
    }

    private static int computeX(Paint paint, String content, int w, int alignment){
        int ret = 0;

        switch (alignment){
        case CENTER:
            ret = w / 2;
            break;

        // ret = 0
        case LEFT:
            break;

        case RIGHT:
            ret = w;
            break;

        /*
         * Default is align left.
         * Should be same as newPaint().
         */
        default:
            break;
        }

        return ret;
    }

    private static class TextProperty {
        int maxWidth;     // max width of lines
        int totalHeight;  // height of all lines
        int heightPerLine;
        String[] lines;

        TextProperty(int w, int h, String[] lines) {
            this.maxWidth = w;
            this.heightPerLine = h;
            this.totalHeight = h * lines.length;
            this.lines = lines;
        }
    }

    private static TextProperty computeTextProperty(
            String content, Paint paint, int maxWidth, int maxHeight) {

        FontMetricsInt fm = paint.getFontMetricsInt();
        int h = (int) Math.ceil(fm.bottom - fm.top);
        int maxContentWidth = 0;

        String[] lines = splitString(content, maxHeight, maxWidth, paint);

        if (maxWidth != 0) {
            maxContentWidth = maxWidth;
        } else {
            // Compute the max width
            int temp = 0;
            for (String line : lines){
                temp = (int) Math.ceil(paint.measureText(line, 0, line.length()));
                if (temp > maxContentWidth){
                    maxContentWidth = temp;
                }
            }
        }

        return new TextProperty(maxContentWidth, h, lines);
    }

    /**
     * If maxWidth or maxHeight is not 0,
     * split the string to fix the maxWidth and maxHeight.
     */
    private static String[] splitString(String content, int maxHeight, int maxWidth,
            Paint paint){
        String[] lines = content.split("\\n");
        String[] ret = null;
        FontMetricsInt fm = paint.getFontMetricsInt();
        int heightPerLine = (int) Math.ceil(fm.bottom - fm.top);
        int maxLines = maxHeight / heightPerLine;

        if (maxWidth != 0) {
            LinkedList<String> strList = new LinkedList<String>();
            for (String line : lines) {
                // The width of line exceeds maxWidth, should divide it into
                // two or more lines
                int lineWidth = (int) Math.ceil(paint.measureText(line));
                if (lineWidth > maxWidth) {
                    strList.addAll(divideStringWithMaxWidth(paint, line, maxWidth));
                } else {
                    strList.add(line);
                }

                // Should not exceed the max height
                if (maxLines > 0 && strList.size() >= maxLines) {
                    break;
                }
            }

            // Remove exceeding lines
            if (maxLines > 0 && strList.size() > maxLines) {
                while (strList.size() > maxLines){
                    strList.removeLast();
                }
            }

            ret = new String[strList.size()];
            strList.toArray(ret);
        } else if (maxHeight != 0 && lines.length > maxLines) {
            // Remove exceeding lines
            LinkedList<String> strList = new LinkedList<String>();
            for (int i = 0; i < maxLines; i++) strList.add(lines[i]);
            ret = new String[strList.size()];
            strList.toArray(ret);
        } else {
            ret = lines;
        }

        return ret;
    }

    private static LinkedList<String> divideStringWithMaxWidth(
            Paint paint, String content, int width) {

        int charLength = content.length();
        int start = 0;
        int tempWidth = 0;
        LinkedList<String> strList = new LinkedList<String>();

        // Break a String into String[] by the width & should wrap the word
        for (int i = 1; i <= charLength; ++i) {
            tempWidth = (int) Math.ceil(paint.measureText(content, start, i));
            if (tempWidth >= width) {
                int lastIndexOfSpace = content.substring(0, i).lastIndexOf(" ");

                if (lastIndexOfSpace != -1 && lastIndexOfSpace > start){
                    // Should wrap the word
                    strList.add(content.substring(start, lastIndexOfSpace));
                    i = lastIndexOfSpace + 1;
                } else {
                    // Should not exceed the width
                    if (tempWidth > width){
                        strList.add(content.substring(start, i - 1));
                        // Compute from previous char
                        --i;
                    } else {
                        strList.add(content.substring(start, i));
                    }
                }

                start = i;
            }
        }

        /*
         * Add the last chars
         */
        if (start < charLength){
            strList.add(content.substring(start));
        }

        return strList;
    }

    private static Paint newPaint(String fontName, int fontSize, String textColor, int align) {
        Paint paint = new Paint();
        paint.setColor(Color.parseColor(textColor));
        paint.setTextSize(fontSize);
        paint.setAntiAlias(true);

        if (fontName != null) paint.setTypeface(TypefaceCache.get(fontName));

        switch (align) {
        case CENTER:
            paint.setTextAlign(Align.CENTER);
            break;

        case LEFT:
            paint.setTextAlign(Align.LEFT);
            break;

        case RIGHT:
            paint.setTextAlign(Align.RIGHT);
            break;

        default:
            paint.setTextAlign(Align.LEFT);
            break;
        }

        return paint;
    }

    private static String refactorString(String str){
        // Avoid error when content is ""
        if (str.compareTo("") == 0){
            return " ";
        }

        /*
         * If the font of "\n" is "" or "\n", insert " " in front of it.
         *
         * For example:
         * "\nabc"     -> " \nabc"
         * "\nabc\n\n" -> " \nabc\n \n"
         */
        StringBuilder strBuilder = new StringBuilder(str);
        int start = 0;
        int index = strBuilder.indexOf("\n");
        while (index != -1){
            if (index == 0 || strBuilder.charAt(index -1) == '\n'){
                strBuilder.insert(start, " ");
                start = index + 2;
            } else {
                start = index + 1;
            }

            if (start > strBuilder.length() || index == strBuilder.length()){
                break;
            }

            index = strBuilder.indexOf("\n", start);
        }

        return strBuilder.toString();
    }
}
