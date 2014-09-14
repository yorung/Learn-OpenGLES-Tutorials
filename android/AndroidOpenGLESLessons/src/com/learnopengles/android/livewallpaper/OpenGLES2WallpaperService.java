package com.learnopengles.android.livewallpaper;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

public abstract class OpenGLES2WallpaperService extends GLWallpaperService {
	@Override
	public Engine onCreateEngine() {
		return new OpenGLES2Engine();
	}
	
	class OpenGLES2Engine extends GLWallpaperService.GLEngine {
		static final String TAG = "OpenGLES2Engine";
		
		@Override
		public void onTouchEvent(MotionEvent e) {
			super.onTouchEvent(e);
			switch (e.getAction()) {
			case MotionEvent.ACTION_MOVE:
				Log.v(TAG, "ACTION_MOVE");
				break;
			case MotionEvent.ACTION_DOWN:
				Log.v(TAG, "ACTION_DOWN");
				break;
			case MotionEvent.ACTION_UP:
				Log.v(TAG, "ACTION_UP");
				break;
			}
		}

		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);

			setTouchEventsEnabled(true);
			
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;
			
			if (supportsEs2) 
			{
				// Request an OpenGL ES 2.0 compatible context.
				setEGLContextClientVersion(2);
				
				// On Honeycomb+ devices, this improves the performance when
				// leaving and resuming the live wallpaper.
				setPreserveEGLContextOnPause(true);

				// Set the renderer to our user-defined renderer.
				setRenderer(getNewRenderer());
			} 
			else 
			{
				// This is where you could create an OpenGL ES 1.x compatible
				// renderer if you wanted to support both ES 1 and ES 2.
				return;
			}			
		}
	}	
	
	abstract Renderer getNewRenderer();
}
