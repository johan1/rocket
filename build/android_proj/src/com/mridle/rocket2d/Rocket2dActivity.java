
package com.mridle.rocket2d;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class Rocket2dActivity extends Activity {
	public static final String TAG = "Rocket2d";

	public static final int MOTION_TYPE_DOWN = 0;
	public static final int MOTION_TYPE_MOVE = 1;
	public static final int MOTION_TYPE_UP	 = 2;

	public static boolean isCreated = false;

	static {
		System.loadLibrary("###ROCKET_LIB###");
	}

	private class SurfaceCallbacks implements SurfaceHolder.Callback {
		private boolean isSurfaceDestroyed = false;
		public SurfaceHolder holder;

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			this.holder = holder;
			Rocket2dActivity.surfaceChanged(holder.getSurface(), format, width, height);		
		}

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			isSurfaceDestroyed = false;
			this.holder = holder;
			Rocket2dActivity.surfaceCreated(holder.getSurface());
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			this.holder = holder;
			if (!isSurfaceDestroyed) {
				isSurfaceDestroyed = true;
				Rocket2dActivity.surfaceDestroyed(holder.getSurface());
			}
		}

		public void surfaceDestroyed() {
			surfaceDestroyed(holder);
		}
	}

	private SurfaceHolder surfaceHolder;
	private SurfaceCallbacks surfaceCallbacks = new SurfaceCallbacks();

	@Override
	public void onCreate(Bundle icicle) {
		isCreated = true;
		super.onCreate(icicle);
		Log.d(TAG, "JAVA: onCreate: " + this);

		final SurfaceView surfaceView = new SurfaceView(this);
		SurfaceHolder holder = surfaceView.getHolder();
		holder.addCallback(surfaceCallbacks);

		setContentView(surfaceView);
		Rocket2dActivity.activityCreated(getPackageResourcePath(), "");

		surfaceView.setOnTouchListener(new View.OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				PointerHelper.handleMotionEvent(surfaceView.getWidth(), surfaceView.getHeight(), event);
				return true;
			}
		});
	}

	@Override
	public void onPause() {
		Log.d(TAG, "JAVA: onPause: " + this);
		Rocket2dActivity.activityPaused();

		super.onPause();
	}

	@Override
	public void onResume() {
		super.onResume();

		Log.d(TAG, "JAVA: onResume: " + this);
		Rocket2dActivity.activityResumed();
	}

	@Override
	public void onDestroy() {
		Log.d(TAG, "JAVA: onDestroy: " + this);

		surfaceCallbacks.surfaceDestroyed();
		Rocket2dActivity.activityDestroyed();

		super.onDestroy();

		isCreated = false;
	}

	@Override
	public void onStart() {
		Log.d(TAG, "JAVA: onStart: " + this);

		super.onStart();
	}

	@Override
	public void onStop() {
		Log.d(TAG, "JAVA: onStop: " + this);

		super.onStop();
	}

	@Override
	public void onRestart() {
		Log.d(TAG, "JAVA: onRestart: " + this);

		super.onRestart();
	}

	public static native void activityCreated(String apkPath, String dataPath);

	public static native void activityPaused();

	public static native void activityResumed();

	public static native void activityDestroyed();

	public static native void surfaceCreated(Surface surface);

	public static native void surfaceChanged(Surface surface, int format, int width, int height);

	public static native void surfaceDestroyed(Surface surface);

	public static native void onPointerEvent(int pointerId, int pointerType, int actionType, float x, float y);
}
