#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/klog.h>

#include "KLogger.h"

// For klogctl()
#define SYSLOG_ACTION_READ          2
#define SYSLOG_ACTION_READ_ALL      3
#define SYSLOG_ACTION_CONSOLE_LEVEL 8
#define SYSLOG_ACTION_SIZE_BUFFER   10

KLogger::KLogger(FileSaver &saver) : Logger(saver) {}

bool KLogger::kSetLevel(int level) {
    /*
        KERN_EMERG             0        System is unusable
        KERN_ALERT             1        Action must be taken immediately
        KERN_CRIT              2        Critical conditions
        KERN_ERR               3        Error conditions
        KERN_WARNING           4        Warning conditions
        KERN_NOTICE            5        Normal but significant condition
        KERN_INFO              6        Informational
        KERN_DEBUG             7        Debug-level messages
    */
    //
    return (klogctl(8, NULL, (long) level) == 0);
}

int KLogger::kGetRingBufSize() {
    return klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
}

int KLogger::kRead(char *buf, int len) {
    return klogctl(SYSLOG_ACTION_READ, buf, (long) len);
}

int KLogger::kReadAll(char *buf, int len) {
    return klogctl(SYSLOG_ACTION_READ_ALL, buf, (long) len);
}

int KLogger::go() {
    int bufsize = kGetRingBufSize();
    char *buf = (char*)malloc(bufsize);
    int n = kReadAll(buf, bufsize);
    while(1) {
        mSaver.save(buf, n);
        n = kRead(buf, 4*1024);
    }
    return 0;
}

void KLogger::stop() {
}
