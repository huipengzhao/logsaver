LOCAL_PATH := $(call my-dir)

logsaver_CFLAGS := \
  -D_FILE_OFFSET_BITS=64 \
  -Werror -Wall -Wno-unused-parameter

# Build the command line tool.
include $(CLEAR_VARS)
LOCAL_MODULE := logsaver
LOCAL_CFLAGS += $(logsaver_CFLAGS)
LOCAL_SHARED_LIBRARIES := libutils liblog
LOCAL_SRC_FILES := main.cpp LogSaver.cpp
LOCAL_INIT_RC := logsaver.rc
include $(BUILD_EXECUTABLE)
