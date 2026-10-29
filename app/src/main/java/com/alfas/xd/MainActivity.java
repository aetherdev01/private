package com.alfas.xd;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.Gravity;
import android.view.MotionEvent;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.graphics.Color;
import android.graphics.Typeface;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {
    private LinearLayout root;
    private TextView status;
    private MenuSurface menuSurface;
    private boolean libraryLoaded = false;

    private static native void nativeSetConfigPath(String path);
    private static native void nativeInit();
    private static native void nativeResize(int width, int height);
    private static native void nativeTouch(float x, float y, boolean down);
    private static native void nativeRender();
    private static native void nativeShutdown();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        showLauncher();
    }

    private int dp(float value) {
        return (int) (value * getResources().getDisplayMetrics().density + 0.5f);
    }

    private TextView makeText(String text, float size, boolean bold) {
        TextView view = new TextView(this);
        view.setText(text);
        view.setTextSize(size);
        view.setTextColor(Color.WHITE);
        if (bold) view.setTypeface(Typeface.DEFAULT, Typeface.BOLD);
        return view;
    }

    private void showLauncher() {
        root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setGravity(Gravity.CENTER_VERTICAL);
        root.setPadding(dp(24), dp(24), dp(24), dp(24));
        root.setBackgroundColor(Color.rgb(18, 18, 18));

        TextView title = makeText("RAMBO", 28, true);
        root.addView(title, new LinearLayout.LayoutParams(-1, -2));

        TextView subtitle = makeText("RAMBO ImGui menu preview", 16, false);
        subtitle.setTextColor(Color.LTGRAY);
        LinearLayout.LayoutParams subtitleParams = new LinearLayout.LayoutParams(-1, -2);
        subtitleParams.topMargin = dp(8);
        root.addView(subtitle, subtitleParams);

        status = makeText("Library not loaded", 14, false);
        status.setTextColor(Color.LTGRAY);
        LinearLayout.LayoutParams statusParams = new LinearLayout.LayoutParams(-1, -2);
        statusParams.topMargin = dp(22);
        root.addView(status, statusParams);

        Button loadButton = new Button(this);
        loadButton.setText("Load .so and open menu preview");
        LinearLayout.LayoutParams buttonParams = new LinearLayout.LayoutParams(-1, -2);
        buttonParams.topMargin = dp(16);
        root.addView(loadButton, buttonParams);
        loadButton.setOnClickListener(v -> loadPreview());

        TextView note = makeText("In the preview, tap the floating RAMBO logo to open or close the tabbed ImGui menu. Demo switches do not modify a game process.", 13, false);
        note.setTextColor(Color.GRAY);
        LinearLayout.LayoutParams noteParams = new LinearLayout.LayoutParams(-1, -2);
        noteParams.topMargin = dp(18);
        root.addView(note, noteParams);
        setContentView(root);
    }

    private void loadPreview() {
        if (!libraryLoaded) {
            try {
                System.loadLibrary("aetherx_ui");
                libraryLoaded = true;
                nativeSetConfigPath(getFilesDir().getAbsolutePath() + "/rambo-preview.ini");
            } catch (UnsatisfiedLinkError error) {
                status.setText("Failed to load libaetherx_ui.so: " + error.getMessage());
                return;
            } catch (SecurityException error) {
                status.setText("Loading native library was blocked: " + error.getMessage());
                return;
            }
        }
        showPreview();
    }

    private void showPreview() {
        root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.rgb(18, 18, 18));

        Button back = new Button(this);
        back.setText("Back");
        root.addView(back, new LinearLayout.LayoutParams(-1, -2));
        back.setOnClickListener(v -> {
            if (menuSurface != null) {
                menuSurface.queueEvent(MainActivity::nativeShutdown);
                menuSurface.onPause();
                menuSurface = null;
            }
            showLauncher();
            status.setText("Loaded libaetherx_ui.so");
        });

        menuSurface = new MenuSurface();
        root.addView(menuSurface, new LinearLayout.LayoutParams(-1, 0, 1.0f));
        setContentView(root);
    }

    private final class MenuSurface extends GLSurfaceView {
        private boolean pointerDown = false;

        MenuSurface() {
            super(MainActivity.this);
            setEGLContextClientVersion(3);
            setEGLConfigChooser(8, 8, 8, 8, 16, 0);
            setPreserveEGLContextOnPause(true);
            setRenderer(new Renderer() {
                @Override
                public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                    nativeShutdown();
                    nativeInit();
                }

                @Override
                public void onSurfaceChanged(GL10 gl, int width, int height) {
                    nativeResize(width, height);
                }

                @Override
                public void onDrawFrame(GL10 gl) {
                    android.opengl.GLES30.glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
                    android.opengl.GLES30.glClear(android.opengl.GLES30.GL_COLOR_BUFFER_BIT);
                    nativeRender();
                }
            });
            setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            int action = event.getActionMasked();
            if (action == MotionEvent.ACTION_DOWN || action == MotionEvent.ACTION_POINTER_DOWN) {
                pointerDown = true;
            } else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_POINTER_UP
                    || action == MotionEvent.ACTION_CANCEL) {
                pointerDown = false;
            }
            final float x = event.getX();
            final float y = event.getY();
            final boolean down = pointerDown;
            queueEvent(() -> nativeTouch(x, y, down));
            return true;
        }
    }

    @Override
    protected void onPause() {
        if (menuSurface != null) menuSurface.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (menuSurface != null) menuSurface.onResume();
    }
}
