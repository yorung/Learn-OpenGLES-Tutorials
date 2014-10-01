package com.learnopengles.android.rbgrnlivewallpaper;

public class CommonJNI {
	static {
		System.loadLibrary("CommonJNI");
	}
	public static native void init();
	public static native void destroy();
	public static native void update(float aspect, float offset);
	public static native void createRipple(float x, float y);
}
