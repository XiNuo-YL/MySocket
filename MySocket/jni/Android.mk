#******************************************************
# Carrie created in 2019/12/17
# All Rights Reserved spurious edition
# ******************************************************/
LOCAL_PATH := $(call my-dir)

$(warning $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_MODULE    := libmysocket

LOCAL_SRC_FILES :=libmysocket_jni.cpp\

LOCAL_CFLAGS := -std=c++11

LOCAL_LDLIBS += -llog -lz -landroid

APP_PLATFORM := 19
include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
