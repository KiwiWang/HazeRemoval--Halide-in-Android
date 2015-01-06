LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := native
LOCAL_ARM_MODE  := arm
LOCAL_SRC_FILES := native.cpp
LOCAL_LDFLAGS   := -Ljni
#LOCAL_LDLIBS    := -lm -llog -landroid jni/halide_generated_$(TARGET_ARCH_ABI)/halide_generated.o # -lOpenCL -lllvm-a3xx
LOCAL_LDLIBS    := -lm -llog -landroid jni/halide_generated_/halide_generated.o # -lOpenCL -lllvm-a3xx
LOCAL_STATIC_LIBRARIES := android_native_app_glue
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include $(LOCAL_PATH)/halide_generated_$(TARGET_ARCH_ABI)/
LOCAL_C_INCLUDES := /home/kiwi/Documents/halide/include /home/kiwi/Documents/Android-Bigdata-files/final/HelloAndroid/jni/halide_generated_/

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
