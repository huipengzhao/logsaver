#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "LogCfg.h"
#include "LogSaver.h"

using namespace logsaver;

int main(int argc, char *argv[]) {
    LogCfg cfg;
    if (!cfg.parse(argc, argv)) {
        cfg.show();
        cfg.showUsage();
        return -EINVAL;
    }

    if (cfg.mLogType == LogCfg::LOGTYPE_HELP) {
        cfg.showUsage();
        return 0;
    }

    LogSaver logsaver;
    return logsaver.run(cfg);
}
