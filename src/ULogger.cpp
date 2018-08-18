#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <string>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <linux/netlink.h>

#define LS_DEBUG
#include "utils.h"
#include "ULogger.h"
#include "uevent.h"

#define UEVENT_MSG_LEN 2048

using namespace logsaver;
using namespace std;

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

struct Uevent {
    string action;
    string path;
    string subsystem;
    string firmware;
    string partition_name;
    string device_name;
    int partition_num;
    int major;
    int minor;
};

static bool uevent_read(int fd, Uevent* uevent) {
    char msg[UEVENT_MSG_LEN + 2];
    int n = uevent_kernel_multicast_recv(fd, msg, UEVENT_MSG_LEN);
    if (n <= 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            L("Error reading from Uevent Fd");
        }
        return false;
    }
    if (n >= UEVENT_MSG_LEN) {
        L("Uevent overflowed buffer, discarding");
        // Return true here even if we discard as we may have more uevents pending and we
        // want to keep processing them.
        return true;
    }

    msg[n] = '\0';
    msg[n + 1] = '\0';

    //ParseEvent(msg, uevent);

    return true;
}

ULoggerPriv::ULoggerPriv(ULogger *logger) : mLogger(logger) {}

int ULoggerPriv::doLogging() {
    L("start");

    int fd = uevent_open_socket(1024*512, true);

    if (fd > 0) {
        int count = 0;
        Uevent uevent;
        while (!mStop) {
            uevent_read(fd, &uevent);
            count++;
            L("A uevent has been read. count=%d", count);
        }
        close(fd);
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

