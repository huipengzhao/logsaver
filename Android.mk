LOCAL_PATH := $(call my-dir)

logsaver_CFLAGS        := -Werror -Wall -Wno-unused-parameter

# Build the command line tool.
include $(CLEAR_VARS)
LOCAL_MODULE           := logsaver
LOCAL_INIT_RC          := logsaver.rc
LOCAL_CFLAGS           := $(logsaver_CFLAGS)
LOCAL_SRC_FILES        := \
	src/LogCfg.cpp \
	src/LogSaver.cpp \
	src/KLogger.cpp \
	src/ULogger.cpp \
	src/uevent.cpp \
	src/FileSavers.cpp \
	src/main.cpp
LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libcutils

## For low version Android such as Android-KK
MY_STL_INC = external/stlport/stlport
ifeq ($(MY_STL_INC), $(wildcard $(MY_STL_INC)))
LOCAL_C_INCLUDES       += external/stlport/stlport bionic
LOCAL_SHARED_LIBRARIES += libstlport
endif

include $(BUILD_EXECUTABLE)
