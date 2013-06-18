package js.g;

import java.util.ArrayList;

import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

public class Stage extends SurfaceView implements SurfaceHolder.Callback {
    private static Stage instance;

    public static Stage getInstance() {
        return instance;
    }

    //--------------------------------------------------------------------------

    private View   splash;
    private String mainScript;

    private SurfaceHolder surfaceHolder;
    private boolean       running;

    private ArrayList<Runnable> queuedEvents = new ArrayList<Runnable>();

    private native static void nativeSetSurface(Surface surface);
    private native static void nativeUnsetSurface();
    private native static void nativeOnDrawFrame(
            int[] touchActions, int[] touchXs, int[] touchYs, int numTouches);

    //--------------------------------------------------------------------------

    private static final int TOUCH_DOWN = 0;
    private static final int TOUCH_MOVE = 1;
    private static final int TOUCH_UP   = 2;
    private static final int TOUCH_TAP  = 3;

    private static final int MAX_TOUCHES = 20;

    private int numTouches  = 0;
    private int numTouches2 = 0;

    private int[] touchActions = new int[MAX_TOUCHES];
    private int[] touchXs      = new int[MAX_TOUCHES];
    private int[] touchYs      = new int[MAX_TOUCHES];

    private int[] touchActions2 = new int[MAX_TOUCHES];
    private int[] touchXs2      = new int[MAX_TOUCHES];
    private int[] touchYs2      = new int[MAX_TOUCHES];

    private GestureDetector gd;
    private Object          touchLock = new Object();

    //--------------------------------------------------------------------------

    public Stage(View splash, String mainScript) {
        super(JSGActivity.getInstance());
        instance = this;

        this.splash     = splash;
        this.mainScript = mainScript;

        gd = new GestureDetector(getContext(), new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onSingleTapUp(MotionEvent e) {
                touchActions[numTouches] = TOUCH_TAP;
                touchXs     [numTouches] = (int) e.getX();
                touchYs     [numTouches] = (int) e.getY();
                numTouches++;
                return true;
            }
        });

        surfaceHolder = getHolder();
        surfaceHolder.addCallback(this);
    }

    public void surfaceChanged(final SurfaceHolder holder, int format, final int width, final int height) {
        Surface surface = holder.getSurface();
        nativeSetSurface(surface);

        if (!JSG.isReady()) {
            // Display splash
            FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                    FrameLayout.LayoutParams.MATCH_PARENT,
                    FrameLayout.LayoutParams.MATCH_PARENT);
            JSGActivity.getInstance().addContentView(splash, lp);

            // Start game thread
            new Thread(new Runnable() {
                public void run() {
                    JSG.init(width, height, mainScript);

                    // Remove splash
                    JSG.runOnUiThread(new Runnable() {
                        public void run() {
                            // http://stackoverflow.com/questions/3080577/removing-a-view-from-an-activity
                            ViewGroup vg = (ViewGroup) (splash.getParent());
                            vg.removeView(splash);
                        }
                    });

                    gameLoop();
                }
            }).start();
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        running = true;
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeUnsetSurface();
        running = false;
    }

    private void gameLoop() {
        while (true) {
            if (running) {
                synchronized (queuedEvents) {
                    for (Runnable runnable : queuedEvents) {
                        runnable.run();
                    }
                    queuedEvents.clear();
                }

                synchronized (touchLock) {
                    for (int i = 0; i < numTouches; i++) {
                        touchActions2[i] = touchActions[i];
                        touchXs2     [i] = touchXs     [i];
                        touchYs2     [i] = touchYs     [i];
                    }
                    numTouches2 = numTouches;
                    numTouches = 0;
                }
                nativeOnDrawFrame(touchActions2, touchXs2, touchYs2, numTouches2);
            } else {
                try { Thread.sleep(500); } catch (Exception e) {}
            }
        }
    }

    public void queueEvent(Runnable runnable) {
        synchronized (queuedEvents) {
            queuedEvents.add(runnable);
        }
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) {
        synchronized (touchLock) {
            if (numTouches < MAX_TOUCHES - 1) {
                // "tap" is fired before "up"
                gd.onTouchEvent(event);

                int x = (int) event.getX();
                int y = (int) event.getY();

                int action = event.getAction();
                if (action == MotionEvent.ACTION_DOWN) {
                    touchActions[numTouches] = TOUCH_DOWN;
                    touchXs     [numTouches] = x;
                    touchYs     [numTouches] = y;
                    numTouches++;
                } else if (action == MotionEvent.ACTION_UP) {
                    touchActions[numTouches] = TOUCH_UP;
                    touchXs     [numTouches] = x;
                    touchYs     [numTouches] = y;
                    numTouches++;
                } else if (action == MotionEvent.ACTION_MOVE) {
                    touchActions[numTouches] = TOUCH_MOVE;
                    touchXs     [numTouches] = x;
                    touchYs     [numTouches] = y;
                    numTouches++;
                }
            }
        }

        return true;
    }
}
