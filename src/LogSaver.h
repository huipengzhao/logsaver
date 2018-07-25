#ifndef LOGSAVER_H
#define LOGSAVER_H

#include "LogCfg.h"

namespace logsaver {

/**
 * Abstract class to save buffers into files.
 */
class FileSaver {
public:
    FileSaver() {};
    virtual ~FileSaver() {};
    virtual int save(const char* buf, int bytes) = 0;
    virtual void finish() = 0;
};

/**
 * Abstract class to produce log buffers.
 */
class Logger {
public:
    Logger(FileSaver *saver) : mSaver(saver) {}
    virtual ~Logger() {};
    virtual int go() = 0;
    virtual void stop() = 0;

protected:
    FileSaver *mSaver;
};

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
