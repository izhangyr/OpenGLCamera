LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)
LOCAL_MODULE := fdk-aac
LOCAL_SRC_FILES := fdk-aac-0.1.3/build-android/master/arm/lib/libfdk-aac.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/fdk-aac-0.1.3/build-android/master/arm/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)

include $(CLEAR_VARS)
LOCAL_MODULE := fdk-aac
LOCAL_SRC_FILES := fdk-aac-0.1.3/build-android/master/arm/lib/libfdk-aac.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/fdk-aac-0.1.3/build-android/master/arm/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),x86)

include $(CLEAR_VARS)
LOCAL_MODULE := fdk-aac
LOCAL_SRC_FILES := fdk-aac-0.1.3/build-android/master/x86/lib/libfdk-aac.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/fdk-aac-0.1.3/build-android/master/x86/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := x264/build-android/master/arm/lib/libx264.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/x264/build-android/master/arm/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := x264/build-android/master/arm/lib/libx264.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/x264/build-android/master/arm/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),x86)

include $(CLEAR_VARS)
LOCAL_MODULE := x264
LOCAL_SRC_FILES := x264/build-android/master/x86/lib/libx264.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/x264/build-android/master/x86/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)
LOCAL_MODULE := rtmp
LOCAL_SRC_FILES := rtmpdump/build/master/arm/lib/librtmp.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/rtmpdump/build/master/arm/include
LOCAL_EXPORT_LDLIBS := -lstdc++
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)

include $(CLEAR_VARS)
LOCAL_MODULE := rtmp
LOCAL_SRC_FILES := rtmpdump/build/master/arm/lib/librtmp.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/rtmpdump/build/master/arm/include
LOCAL_EXPORT_LDLIBS := -lstdc++
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),x86)

include $(CLEAR_VARS)
LOCAL_MODULE := rtmp
LOCAL_SRC_FILES := rtmpdump/build/master/x86/lib/librtmp.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/rtmpdump/build/master/x86/include
LOCAL_EXPORT_LDLIBS := -lstdc++
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := ffmpeg/build/master/arm/libffmpeg.so
LOCAL_CFLAGS := -D__STDINT_MACROS
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/ffmpeg/build/master/arm/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := ffmpeg/build/master/armv7/libffmpeg.so
LOCAL_CFLAGS := -D__STDINT_MACROS
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/ffmpeg/build/master/armv7/include
include $(PREBUILT_SHARED_LIBRARY)

endif

ifeq ($(TARGET_ARCH_ABI),x86)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := ffmpeg/build/master/x86/libffmpeg.so
LOCAL_CFLAGS := -D__STDINT_MACROS
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/ffmpeg/build/master/x86/include
include $(PREBUILT_SHARED_LIBRARY)

endif


