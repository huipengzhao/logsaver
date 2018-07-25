LOCAL_PATH := $(call my-dir)

logsaver_CFLAGS := \
  -D_FILE_OFFSET_BITS=64 \
  -Werror -Wall -Wno-unused-parameter

# Build the command line tool.
include $(CLEAR_VARS)

LOCAL_MODULE           := logsaver

LOCAL_INIT_RC          := logsaver.rc

LOCAL_CFLAGS           += $(logsaver_CFLAGS)

LOCAL_SHARED_LIBRARIES := \
                          liblog \
                          libutils \

LOCAL_SRC_FILES        := \
                          src/LogCfg.cpp \
                          src/LogSaver.cpp \
			  src/KLogger.cpp \
                          src/main.cpp \

include $(BUILD_EXECUTABLE)
