#ifndef LOGSAVER_H
#define LOGSAVER_H

//-----------------------------------------------------------------------------
// LOG UTILS
//-----------------------------------------------------------------------------
#include <stdio.h>
// Here we use stderr because stdout maybe used as default saver.
#define LSLOG(fmt, args...) fprintf(stderr, "%s %d: " fmt "\n", __FUNCTION__, __LINE__, ##args);

//-----------------------------------------------------------------------------
// BASIC DEFINITIONS
//-----------------------------------------------------------------------------
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
