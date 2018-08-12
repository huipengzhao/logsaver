#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/klog.h>

#define LS_DEBUG
#include "utils.h"
#include "KLogger.h"

using namespace logsaver;

namespace logsaver {

// For klogctl()
#define SYSLOG_ACTION_READ          2
#define SYSLOG_ACTION_READ_ALL      3
#define SYSLOG_ACTION_CONSOLE_LEVEL 8
#define SYSLOG_ACTION_SIZE_BUFFER   10

class KLoggerPriv {
public:
    KLoggerPriv(KLogger *logger);

    KLogger * mLogger;
    pthread_t mThrd;
    int       mBufsize;
    char *    mBuf;

    int doLogging();
    int kGetRingBufSize();
    int kRead(char *buf, int len);
    int kReadAll(char *buf, int len);
};

} //namespace logsaver

// Signal handler for child thread.
static void klog_sighandler(int sig) {
    if (sig==SIGUSR1) {
        L("Exit the child thread.");
        pthread_exit(NULL);
    }
}

// Register the signal handler.
static void klog_register_sighandler() {
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = klog_sighandler;
    sigaction(SIGUSR1, &action, NULL);
}

// Child thread main entry, child thread does the klogctrl.
static void *klog_thread(void *data) {
    KLoggerPriv *priv = (KLoggerPriv *)data;
    if (priv == NULL) { return (void*)(-EINVAL); }
    long ret = priv->doLogging();
    return (void*)ret;
}

int KLoggerPriv::kGetRingBufSize() {
    return klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
}

KLoggerPriv::KLoggerPriv(KLogger *logger) : mLogger(logger) {}

int KLoggerPriv::kRead(char *buf, int len) {
    /*
     * NOTE: SYSLOG_ACTION_READ needs CAP_SYSLOG capability.
     * Several ways to overcome this:
     * 1) Android supports capabilities setting in init.rc.
     * 2) Invoke some cap setting tools to set cap to the process dynamically.
     * 3) Read from /proc/kmsg instead of using klogctl(SYSLOG_ACTION_READ).
     *    Change owner and mode of /proc/kmsg, allow the process to access it.
     * 4) Run the process as root or sudo, not recommended.
    */
    return klogctl(SYSLOG_ACTION_READ, buf, (long) len);
}

int KLoggerPriv::kReadAll(char *buf, int len) {
    return klogctl(SYSLOG_ACTION_READ_ALL, buf, (long) len);
}

int KLoggerPriv::doLogging() {
    L("start");

    klog_register_sighandler();

    int n = kReadAll(mBuf, mBufsize);
    while(n > 0) {
        if (mLogger->getFileSaver()) {
            mLogger->getFileSaver()->save(mBuf, n);
        }
        n = kRead(mBuf, mBufsize);
        if (n < 0) {
            L("kRead() returns %d: %s", n, strerror(errno));
        }
    }
    L("exit");
    return (n < 0 ? n : 0);
}

KLogger::KLogger() : Logger(), mPriv(new KLoggerPriv(this)) {}

// override
KLogger::~KLogger() { delete mPriv; }

// override
int KLogger::go() {
    L("start");
    mPriv->mBufsize = mPriv->kGetRingBufSize();
    mPriv->mBuf = (char*)malloc(mPriv->mBufsize);

    // Init the saver before anything.
    if (!getFileSaver()) {
        // Cry out if no saver.
        L("Warning! KLogger is started without any file saver.");
    }

    /*
        The klogctl() function will block in uninterruptable state (why?),
        So a handled SIGINT is not going to interrupt it. So that it will
        not respond to the Ctrl+C keypress.
        Here we put it in a child thread, and stop it by raising a SIGUSR1
        signal and exiting the child thread in the signal handler.
    */
    int err = pthread_create(&mPriv->mThrd, NULL, klog_thread, mPriv);
    if (err) return err;

    // Wait for the child thread.
    long ret = 0;
    pthread_join(mPriv->mThrd, (void**)&ret);
    L("child thread exit with return value %d", (int)ret);

    // Release the resource.
    if (mPriv->mBuf) free(mPriv->mBuf);

    L("exit");
    return (int)ret;
}

// override
void KLogger::stop() {
    L("Stop the child thread.");
    pthread_kill(mPriv->mThrd, SIGUSR1);
}
