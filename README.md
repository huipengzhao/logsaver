# Logsaver
A log saving tool for Android platform.

## How To Use It
- Copy this project into AOSP code tree, like **${ANDROID_BUILD_TOP}/external/logsaver**.
- Make sure you are in AOSP build environment.
  - Which means you've done envsetup and lunch.
- Add logsaver to PRODUCT_PACKAGES.
  - Mostly in device/\<vendor\>/\<device\>/XXXXX.mk
  - **PRODUCT_PACKAGES += logsaver**
- Cd to logsaver/security and run the **setup.sh** (for Android 8 SELinux).
- make
- Now the followings are what we want.
  - /system/bin/logsaver
  - /system/etc/init/logsaver.rc

## SELinux Configuration for Android 8
See what we have to do in security/setup.sh .
Other Android versions are not adapted for now.

## Tested Platform & Device
- [x] Android 8.1 on Pixel 2
- [ ] Android 4.4 on Nexus 7 (2013)

## The Earliest Timing To Save Logs
In brief, the timing is the same as we 'start logd' or 'load\_persist\_props'.

The userdata partition may be encrypted or nonencrypted. And we can not access a encrypted /data partition until it is decrypted. So search the key word 'nonencrypted' or 'vold.decrypt' in init.rc and see the following actions.

Here I just define the service logsaver in class main, which is started very soon after /data can be accessed.
