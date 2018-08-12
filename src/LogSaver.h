#ifndef LOGSAVER_H
#define LOGSAVER_H

#include "LogCfg.h"
#include "LogSaverBase.h"

namespace logsaver {

/**
 * Concrete class to fetch and save logs.
 * This is a single instance class.
 */
class LogSaverPriv;
class LogSaver {
public:
    LogSaver();
    int run(LogCfg cfg);

private:
    LogSaverPriv *priv;
};

} //namesapce logsaver

#endif //LOGSAVER_H
