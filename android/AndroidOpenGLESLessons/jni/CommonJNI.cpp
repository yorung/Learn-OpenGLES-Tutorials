#include "stdafx.h"

JNIEnv* jniEnv;
//const char* boundJavaClass = "common/pinotnoir/livewallpaper/CommonGLDrawer";
const char* boundJavaClass = "com/learnopengles/android/rbgrnlivewallpaper/CommonGLDrawer";

//package com.learnopengles.android.rbgrnlivewallpaper;
#define FUNC(name) Java_com_learnopengles_android_rbgrnlivewallpaper_CommonJNI_##name
//#define FUNC(name) Java_common_pinotnoir_livewallpaper_CommonJNI_##name


extern "C" {

JNIEXPORT void JNICALL FUNC(init)(JNIEnv* env, jobject obj)
{
	aflog("init called!", obj);

	jniEnv = env;
	glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	waterSurface.Init();
	bgTransferer.Init();
	jniEnv = nullptr;
	aflog("init end!", obj);
}

JNIEXPORT void JNICALL FUNC(createRipple)(JNIEnv* env, jobject obj, jfloat x, jfloat y)
{
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	Vec3 r = transform(Vec3(x, y, 0), inv(matV * matP));
	waterSurface.CreateRipple(Vec2(r.x, r.y));
}

JNIEXPORT void JNICALL FUNC(destroy)(JNIEnv* env, jobject obj)
{
	jniEnv = env;
	texMan.Destroy();
	shaderMan.Destroy();
	waterSurface.Destroy();
	jniEnv = nullptr;
}

JNIEXPORT void JNICALL FUNC(update)(JNIEnv* env, jobject obj, jfloat aspect, jfloat offset)
{
	jniEnv = env;

	double tm1 = GetTime();

	if (aspect < 1) {
		matrixMan.Set(MatrixMan::VIEW, fastInv(translate(0, 0.5f * (1 - aspect), 0)));
		matrixMan.Set(MatrixMan::PROJ, Mat(
			1, 0, 0, 0,
			0, 1 / aspect, 0, 0,
			0, 0, 0.1f, 0,
			0, 0, 0, 1));
	} else {
		matrixMan.Set(MatrixMan::VIEW, fastInv(translate(offset * (1 - 1 / aspect), 0, 0)));
		matrixMan.Set(MatrixMan::PROJ, Mat(
			aspect, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 0.1f, 0,
			0, 0, 0, 1));
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bgTransferer.Update();
	waterSurface.Draw();

	fps.Update();
//	aflog("fps=%f\n", fps.Get());

	double tm2 = GetTime();
//	aflog("%f\n", tm2 - tm1);

	jniEnv = nullptr;
}

}

#ifdef _MSC_VER
#include "SampleApplication.h"
const int SCR_W = 600;
const int SCR_H = 600;
class MyApp : public SampleApplication
{
	int frames;
public:
	MyApp::MyApp() : SampleApplication("Bingmu", SCR_W, SCR_H)
	{
		frames = 0;
	}
	virtual bool initialize()
	{
		Java_common_pinotnoir_livewallpaper_CommonJNI_init((void*)0, (void*)0);
		return true;
	}
	virtual void destroy()
	{
		Java_common_pinotnoir_livewallpaper_CommonJNI_destroy((void*)0, (void*)0);
	}
	virtual void step(float dt, double totalTime)
	{
		double estimated = ++frames * (double)0.0166666;
		if (estimated > totalTime) {
			Sleep((int)((estimated - totalTime) * 1000));
		}
	}
	virtual void draw()
    {
		static bool last;
		bool current = !!(GetKeyState(VK_LBUTTON) & 0x80);
		bool edge = current && !last;
		last = current;

//		if (edge) {
		if (current) {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(GetForegroundWindow(), &pt);
			Java_common_pinotnoir_livewallpaper_CommonJNI_createRipple((void*)0, (void*)0, (float)pt.x / SCR_W * 2 - 1, (float)pt.y / SCR_H * -2 + 1);
		}

		Java_common_pinotnoir_livewallpaper_CommonJNI_update((void*)0, (void*)0, 1, 0);
	}
};

int main(int argc, char **argv)
{
    MyApp app;
    return app.run();
}
#endif
