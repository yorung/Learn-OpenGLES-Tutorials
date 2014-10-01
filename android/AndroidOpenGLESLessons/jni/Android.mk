LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CommonJNI
LOCAL_SRC_FILES := CommonJNI.cpp helper.cpp tex_man.cpp shader_man.cpp water_surface.cpp matrix_man.cpp fps.cpp bg_transferer.cpp
LOCAL_LDLIBS    := -lGLESv2
LOCAL_LDLIBS    += -L$(SYSROOT)/usr/lib -llog

LOCAL_C_INCLUDES += D:\android-ndk-r9d/sources/cxx-stl/stlport/stlport

include $(BUILD_SHARED_LIBRARY)
