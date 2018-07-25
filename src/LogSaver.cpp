#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include "LogSaver.h"
#include "KLogger.h"

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

class DefaultFileSaver : public FileSaver {
public:
    int save(const char *buf, int bytes) {
        return write(fileno(stdout), buf, bytes);
    }
};

//----------
LogSaver::LogSaver(LogCfg &cfg) {
    mCfg = cfg;
}

int LogSaver::run() {
    mCfg.show();

    DefaultFileSaver fsaver;
    KLogger klogger(fsaver);
    klogger.go();

    return 0;
}

