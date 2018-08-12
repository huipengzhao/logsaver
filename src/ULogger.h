#ifndef ULOGGER_H
#define ULOGGER_H

#include "LogSaverBase.h"

namespace logsaver {

class ULoggerPriv;

class ULogger : public Logger {
public:
    ULogger();
    ~ULogger(); //override
    int go(); //override
    void stop(); //override

private:
    ULoggerPriv *mPriv;
};

} //namespace logsaver

#endif //ULOGGER_H
