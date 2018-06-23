#!/bin/bash

set -x

cp private_logsaver.te ${ANDROID_BUILD_TOP}/system/sepolicy/private/logsaver.te

cat private_file_contexts >> ${ANDROID_BUILD_TOP}/system/sepolicy/private/file_contexts

cat private_compat_26.0_26.0.ignore.cil >> ${ANDROID_BUILD_TOP}/system/sepolicy/private/compat/26.0/26.0.ignore.cil

set +x
