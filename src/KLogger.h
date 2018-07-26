#ifndef KLOGGER_H
#define KLOGGER_H

#include "LogSaver.h"

namespace logsaver {

class KLoggerPriv;

class KLogger : public Logger {
public:
    KLogger(FileSaver *saver);
    ~KLogger(); //override
    int go(); //override
    void stop(); //override

    void kLogThread();

private:
    int kGetRingBufSize();
    int kRead(char *buf, int len);
    int kReadAll(char *buf, int len);

private:
    KLoggerPriv *mPriv;
};

} //namespace logsaver

#endif //KLOGGER_H
