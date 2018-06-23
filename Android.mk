LOCAL_PATH := $(call my-dir)

logsaver_CFLAGS := \
  -D_FILE_OFFSET_BITS=64 \
  -Werror -Wall

# Build the command line tool.
include $(CLEAR_VARS)
LOCAL_MODULE := logsaver
LOCAL_CFLAGS += $(logsaver_CFLAGS)
LOCAL_SHARED_LIBRARIES := libutils liblog
LOCAL_SRC_FILES := logsaver.cpp
LOCAL_INIT_RC := init.logsaver.rc
include $(BUILD_EXECUTABLE)
