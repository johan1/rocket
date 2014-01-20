package com.mridle.rocket2d;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.View;
import android.view.MotionEvent;

import android.util.Log;

public class LauncherActivity extends Activity {
	private static final String TAG = "Rocket2d";
	private static final long DELAY = 50;

	private boolean cancelled = false;
	private final Handler handler = new Handler();

	public final Runnable launcher = new Runnable() {
		@Override
		public void run() {
			if (cancelled) {
				return;
			}

			if (!Rocket2dActivity.isCreated) {
				Log.d(TAG, "JAVA Launching rocket2d activity");
				startActivity(new Intent(LauncherActivity.this, Rocket2dActivity.class));
				finish();
			} else {
				Log.d(TAG, "JAVA Previous activity not destroyed yet, run later");
				handler.postDelayed(this, DELAY);
			}
		}
	};

	@Override
	public void onResume() {
		super.onResume();

		cancelled = false;
		handler.post(launcher);
	}

	@Override
	public void onPause() {
		cancelled = true;
		super.onPause();
	}
}
