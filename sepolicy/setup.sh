#!/bin/bash

set -x

cp private/logsaver.te ${ANDROID_BUILD_TOP}/system/sepolicy/private/logsaver.te

cat private/file_contexts >> ${ANDROID_BUILD_TOP}/system/sepolicy/private/file_contexts

cat private/compat/26.0/26.0.ignore.cil >> ${ANDROID_BUILD_TOP}/system/sepolicy/private/compat/26.0/26.0.ignore.cil

set +x
