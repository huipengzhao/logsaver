#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define LS_DEBUG
#include "utils.h"
#include "ULogger.h"

using namespace logsaver;

namespace logsaver {

class ULoggerPriv {
public:
    ULoggerPriv(ULogger *logger);

    ULogger * mLogger;
    int       mBufsize;
    char *    mBuf;
    volatile bool mStop;

    int doLogging();
};

} //namespace logsaver

ULoggerPriv::ULoggerPriv(ULogger *logger) : mLogger(logger) {}

int ULoggerPriv::doLogging() {
    L("start");

    //ulog_register_sighandler();

    // loop
    while (!mStop) {
        sleep(1);
    }

    L("exit");
    return 0;
}

ULogger::ULogger() : Logger(), mPriv(new ULoggerPriv(this)) {}

// override
ULogger::~ULogger() { delete mPriv; }

// override
int ULogger::go() {
    L("start");
    mPriv->mBufsize = 4096;
    mPriv->mBuf = (char*)malloc(mPriv->mBufsize);

    // Init the saver before anything.
    if (!getFileSaver()) {
        // Cry out if no saver.
        L("Warning! ULogger is started without any file saver.");
    }

    // Logging the UEvents.
    int ret = 0;
    ret = mPriv->doLogging();

    // Release the resource.
    if (mPriv->mBuf) free(mPriv->mBuf);

    L("exit");
    return (int)ret;
}

// override
void ULogger::stop() {
    L("Stop the child thread.");
    mPriv->mStop = true;
}

