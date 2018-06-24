#include <errno.h>

#include "LogSaver.h"

int main(int argc, char *argv[]) {
    LogCfg cfg;
    if (!cfg.parse(argc, argv)) {
        cfg.showUsage();
        return -EINVAL;
    }

    LogSaver logsaver(cfg);
    return logsaver.run();
}
