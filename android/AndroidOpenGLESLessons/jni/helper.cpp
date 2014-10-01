#include "stdafx.h"

#ifdef _MSC_VER
void *LoadFile(const char *fileName)
{
	bool result = false;
	FILE *f = nullptr;
	int _size;
	void *ptr = NULL;

	if (fopen_s(&f, fileName, "rb")) {
		return nullptr;
	}

	if (fseek(f, 0, SEEK_END)) {
		goto DONE;
	}
	_size = ftell(f);
	if (_size < 0) {
		goto DONE;
	}
	if (fseek(f, 0, SEEK_SET)) {
		goto DONE;
	}
	ptr = calloc(_size + 1, 1);
	if (!ptr) {
		goto DONE;
	}
	if (_size > 0) {
		if (!fread(ptr, _size, 1, f)) {
			goto DONE;
		}
	}
	result = true;
DONE:
	if (f) {
		fclose(f);
	}
	if (result){
		return ptr;
	} else {
		if (ptr) {
			free(ptr);
		}
		return nullptr;
	}
}
#else
void *LoadFile(const char *fileName)
{
	aflog("LoadFile(%s) called!", fileName);

	jclass myview = jniEnv->FindClass(boundJavaClass);
	if (myview == 0) {
		aflog("FindClass(%s) failed!", boundJavaClass);
		return nullptr;
	}
	jmethodID method = jniEnv->GetStaticMethodID(myview, "loadIntoBytes", "(Ljava/lang/String;)[B");
	if (method == 0) {
		aflog("GetStaticMethodID(%s) failed!", boundJavaClass);
		return nullptr;
	}
	aflog("LoadFile(%s) ready!", fileName);

	jobject arrayAsJObject = jniEnv->CallStaticObjectMethod(myview, method, jniEnv->NewStringUTF(fileName));
	jbyteArray array = (jbyteArray)arrayAsJObject;
	aflog("LoadFile(%s) ready!  - 2", fileName);

	jbyte* byteArray = jniEnv->GetByteArrayElements(array, NULL);

	aflog("LoadFile(%s) ready! - 3", fileName);

	jsize arrayLen = jniEnv->GetArrayLength(array);

	void* ptr = calloc(arrayLen + 1, 1);
	memcpy(ptr, byteArray, arrayLen);

	jniEnv->ReleaseByteArrayElements(array, byteArray, 0);

	aflog("LoadFile(%s) succeed!", fileName);
	return ptr;
}
#endif

#ifdef _MSC_VER
double GetTime()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / f.QuadPart;
}
#else
double GetTime()
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec / 1000000000;
}
#endif

float Random()
{
	return (float)rand() / RAND_MAX;
}
