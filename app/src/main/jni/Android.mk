LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := x264/build-android/master/arm/lib/libx264.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := openglcamera
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)   \
    $(LOCAL_PATH)/x264/build-android/master/arm/include \
	
LOCAL_SRC_FILES := \
	OpenGLNative.cpp \
	OpenGL.cpp \
	ColorSpace.cpp \
	OpenGLRender.cpp \
	ImageFilter.cpp \
	Curve.cpp \
	H264Writer.cpp \
	
LOCAL_SHARED_LIBRARIES := \
	x264 \

LOCAL_CFLAGS := -Wall
LOCAL_LDLIBS := -lGLESv2 -llog -Os

include $(BUILD_SHARED_LIBRARY)
