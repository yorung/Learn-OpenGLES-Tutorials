package com.learnopengles.android.rbgrnlivewallpaper;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.util.Log;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;


public class CommonGLDrawer implements GLSurfaceView.Renderer{
	private final static String TAG = "CommonGLDrawer";
	boolean useScroll;
	static Context context;
	EGL10 egl;
	EGLDisplay disp;
	EGLContext eglContext;
	EGLSurface surface;
	SurfaceHolder holder;
	EGLConfig cfg[] = new EGLConfig[1];
	
	public CommonGLDrawer(Context context_) {
		context = context_;
	}

	public void destroy() {
		if (egl != null) {
			Log.v(TAG, "destroyed");
			CommonJNI.destroy();
			egl.eglMakeCurrent(disp, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
			egl.eglDestroySurface(disp, surface);
			egl.eglDestroyContext(disp, eglContext);
			egl.eglTerminate(disp);
			egl = null;
		}
	}

	public boolean isValid() {
		return egl != null;
	}

	public double GetTime()
	{
		return (double)System.nanoTime() / (double)(1000 * 1000 * 1000);
	}
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		CommonJNI.init();
	}
	int sizeX, sizeY;
	public void onSurfaceChanged(GL10 gl, int width_, int height_)
	{
		sizeX = width_;
		sizeY = height_;
	}
	public void onDrawFrame(GL10 gl) {
			GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
			CommonJNI.createRipple((OpenGLES2WallpaperService.touchX / sizeX) * 2 - 1, (OpenGLES2WallpaperService.touchY / sizeY) * -2 + 1);

//			glClearColor(1, 0, 0, 1);
			GLES20.glViewport(0, 0, sizeX, sizeY);
			CommonJNI.update((float)sizeY / sizeX, 0);
//			CommonJNI.update((float)sizeY / sizeX, useScroll ? offset : 1);
//			double t2 = GetTime();
//			egl.eglSwapBuffers(disp, surface);
//			double t3 = GetTime();

//			String str = String.format("%f %f %f\n", t1 - t0, t2 - t1, t3 - t2);
//			Log.e(TAG, str);
	}

	public static byte[] loadIntoBytes(String fileName) {
		Log.e(TAG, "loadIntoBytes");
		AssetManager assetManager = context.getAssets();
		try {
			InputStream is = assetManager.open(fileName);
			byte buf[] = new byte[is.available()];
			is.read(buf);
			Log.e(TAG, "loadIntoBytes succeed!");
			return buf;
		} catch (IOException e) {
			Log.e(TAG, "loadIntoBytes failed!");
		}
		return null;
	}

	public static int loadTexture(String s){
		Log.e(TAG, "loadTexture");
		Bitmap img;
		try {
			img = BitmapFactory.decodeStream(context.getAssets().open(s));
		} catch (IOException e) {
			return -1;
		}
		int tex[] = new int[1];
		GLES20.glGenTextures(1, tex, 0);
		GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, tex[0]);
		GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, img, 0);
		img.recycle();
		Log.e(TAG, "loadTexture succeed!");
		return tex[0];
	}
}
