#ifndef KLOGGER_H
#define KLOGGER_H

#include "LogSaver.h"

class KLogger : public Logger {
public:
    KLogger(FileSaver &saver);
    int go();
    void stop();

private:
    bool kSetLevel(int level);
    int  kGetRingBufSize();
    int  kRead(char *buf, int len);
    int  kReadAll(char *buf, int len);
};

#endif //KLOGGER_H
