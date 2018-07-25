#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "utils.h"
#include "LogSaver.h"
#include "KLogger.h"

static Logger *s_logger = NULL;

void signal_handler(int sig) {
    if(sig==SIGINT){
        LSLOG("Ctrl + C pressed, quit.");
        if (s_logger) {
            s_logger->stop();
        }
    }
}

class DefaultFileSaver : public FileSaver {
public:
    // override
    int save(const char *buf, int bytes) {
        return write(fileno(stdout), buf, bytes);
    }
    // override
    void finish() {};
};

LogSaver::LogSaver(LogCfg &cfg) {
    mCfg = cfg;
}

int LogSaver::run() {
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = signal_handler;
    sigaction(SIGINT, &action, NULL);

    mCfg.show();

    DefaultFileSaver fsaver;
    KLogger klogger(fsaver);

    LSLOG("LogSaver start.");
    s_logger = &klogger;
    s_logger->go();
    s_logger = NULL;
    LSLOG("LogSaver exit.");

    return 0;
}

