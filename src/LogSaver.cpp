#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include "LogSaver.h"

#if 0
static int open_file_index(const char *fpattern, unsigned int index) {
    static int fpath_len = 0;
    static char *fpath = NULL;

    // Re-alloc buffer for file path if necessary.
    int curr_len = strlen(fpattern)+4; //_nnn
    if (fpath_len < curr_len) {
        fpath_len = curr_len;
        fpath = (char*)realloc(fpath, fpath_len);
    }
    if (!fpath) return -ENOMEM;

    // Compose the file path with index.
    snprintf(fpath, fpath_len, "%s_%03u", fpattern, (index%1000));

    return open(fpath, O_WRONLY|O_CREAT|O_TRUNC, 0644);
}
#endif


class Klogger {
public:
    static bool setLevel(int level);
    static int getRingBufSize();
    static int read(char *buf, int len);
    static int readLast(char *buf, int len);
    static void saveToFile(FILE *fp);
};

#include <sys/klog.h>

bool Klogger::setLevel(int level) {
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
    //SYSLOG_ACTION_CONSOLE_LEVEL
    return (klogctl(8, NULL, (long) level) == 0);
}

int Klogger::getRingBufSize() {
    //SYSLOG_ACTION_SIZE_BUFFER
    return klogctl(10, NULL, 0);
}

int Klogger::read(char *buf, int len) {
    //SYSLOG_ACTION_READ
    return klogctl(2, buf, (long) len);
}

int Klogger::readLast(char *buf, int len) {
    //SYSLOG_ACTION_READ_ALL
    return klogctl(3, buf, (long) len);
}

#if 0
int Klogger::saveToFileWithIndexSuffix(const char *filePattern,
                unsigned int fileSize, unsigned int startIndex) {
    if (!filePattern) return -EINVAL;
    if (fileSize < 4096) return -EINVAL;

    int kbufsize = Klogger::getRingBufSize();

    int fd = open_file_index(filePattern, startIndex);

    return 0;
}

int Klogger::saveToFileWithDateSuffix(const char *filePattern, int fileSize) {
    return 0;
}
#endif

void Klogger::saveToFile(FILE * fp) {
    int fd = fileno(fp);
    int bufsize = Klogger::getRingBufSize();
    char *buf = (char*)malloc(bufsize);
    int n = Klogger::readLast(buf, bufsize);
    while(1) {
        write(fd, buf, n);
        n = Klogger::read(buf, 4*1024);
    }
}

LogSaver::LogSaver(LogCfg &cfg) {
    mCfg = cfg;
}

int LogSaver::run() {
    mCfg.show();
    printf("KLOG buffer size: %d\n", Klogger::getRingBufSize());
    fflush(stdout);

    Klogger::saveToFile(stdout);
    return 0;
}

