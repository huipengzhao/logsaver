#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include "LogSaver.h"
#include "KLogger.h"

class DefaultFileSaver : public FileSaver {
public:
    int save(const char *buf, int bytes) {
        return write(fileno(stdout), buf, bytes);
    }
};

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

