# Logsaver
A log saving tool for Android platform.

## Tested Platform & Device
- [x] Android 8.1 on Pixel 2
- [ ] Android 4.4 on Nexus 7 (2013)

## The Earliest Timing To Save Logs
In brief, the timing is the same as we 'start logd' or 'load\_persist\_props'.

The userdata partition may be encrypted or nonencrypted. And we can not access a encrypted /data partition until it is decrypted. So search the key word 'nonencrypted' or 'vold.decrypt' in init.rc and see the following actions.

Here I just define the service logsaver in class main, which is started very soon after /data can be accessed.
