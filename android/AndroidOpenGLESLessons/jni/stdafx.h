#ifdef _MSC_VER
#define NOMINMAX
#include <Windows.h>
#define JNIEXPORT
#define JNICALL
typedef void JNIEnv;
typedef void* jobject;
typedef float jfloat;
#define aflog printf
#define snprintf sprintf_s
#else
#include <jni.h>
#include <android/log.h>
#define nullptr NULL;
extern JNIEnv* jniEnv;
extern const char* boundJavaClass;
#define aflog(fmt,...) __android_log_print(ANDROID_LOG_DEBUG, "MyTag", fmt, __VA_ARGS__)
#endif

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>

#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <deque>

#include <math.h>
#include <assert.h>
#include <time.h>

#include "af_math.h"
#include "helper.h"
#include "tex_man.h"
#include "shader_man.h"
#include "matrix_man.h"
#include "bg_transferer.h"
#include "water_surface.h"
#include "fps.h"
