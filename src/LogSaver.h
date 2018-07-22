#ifndef LOGSAVER_H
#define LOGSAVER_H

#include "LogCfg.h"

class LogSaver {
public:
    LogSaver(LogCfg &cfg);
    int run();

protected:
    LogCfg mCfg;
};

#endif //LOGSAVER_H
