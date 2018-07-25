#ifndef LOGSAVER_H
#define LOGSAVER_H

#include <stdio.h>
#define LSLOG(fmt, args...) printf("%s %d: " fmt "\n", __FUNCTION__, __LINE__, ##args);

#include "LogCfg.h"

class FileSaver {
public:
    FileSaver() {};
    virtual ~FileSaver() {};
    virtual int save(const char* buf, int bytes) = 0;
};

class Logger {
public:
    Logger(FileSaver &saver) : mSaver(saver) {}
    virtual ~Logger() {};
    virtual int go() = 0;
    virtual void stop() = 0;

protected:
    FileSaver &mSaver;
};

class LogSaver {
public:
    LogSaver(LogCfg &cfg);
    int run();

protected:
    LogCfg mCfg;
};

#endif //LOGSAVER_H
