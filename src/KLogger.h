#ifndef KLOGGER_H
#define KLOGGER_H

#include "LogSaverBase.h"

namespace logsaver {

class KLoggerPriv;

class KLogger : public Logger {
public:
    KLogger();
    ~KLogger(); //override
    int go(); //override
    void stop(); //override

private:
    KLoggerPriv *mPriv;
};

} //namespace logsaver

#endif //KLOGGER_H
