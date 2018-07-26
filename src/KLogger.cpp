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

// Signal handlers for child thread
static void klog_sighandler(int sig) {
    if (sig==SIGUSR1) {
        LSLOG("Exit the child thread.");
        pthread_exit(NULL);
    }
}

// Child thread main entry, child thread does the klogctrl.
static void *klog_thread(void *data) {
    KLogger *klogger = (KLogger *)data;
    if (klogger == NULL) { return (void*)(-EINVAL); }
    long ret = klogger->kLogThread();
    return (void*)ret;
}

class KLoggerPriv {
public:
    pthread_t thrd;
    int       bufsize;
    char *    buf;
};

} //namespace logsaver

KLogger::KLogger() : Logger(), mPriv(new KLoggerPriv()) {}

// override
KLogger::~KLogger() { delete mPriv; }

int KLogger::kGetRingBufSize() {
    return klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
}

int KLogger::kRead(char *buf, int len) {
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

int KLogger::kReadAll(char *buf, int len) {
    return klogctl(SYSLOG_ACTION_READ_ALL, buf, (long) len);
}

int KLogger::kLogThread() {
    LSLOG("start");
    int bufsize = mPriv->bufsize;
    char *buf = mPriv->buf;

    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = klog_sighandler;
    sigaction(SIGUSR1, &action, NULL);

    int n = kReadAll(buf, bufsize);
    while(n > 0) {
        if (mSaver) {
            mSaver->save(buf, n);
        }
        n = kRead(buf, bufsize);
        if (n < 0) {
            LSLOG("kRead() returns %d: %s", n, strerror(errno));
        }
    }
    LSLOG("exit");
    return (n < 0 ? n : 0);
}

// override
int KLogger::go() {
    LSLOG("start");
    mPriv->bufsize = kGetRingBufSize();
    mPriv->buf = (char*)malloc(mPriv->bufsize);

    // Init the saver before anything.
    if (mSaver) {
        mSaver->prepare();
    }

    /*
        The klogctl() function will block in uninterruptable state (why?),
        So a handled SIGINT is not going to interrupt it. So that it will
        not respond to the Ctrl+C keypress.
        Here we put it in a child thread, and stop it by raising a SIGUSR1
        signal and exiting the child thread in the signal handler.
    */
    int err = pthread_create(&mPriv->thrd, NULL, klog_thread, this);
    if (err) return err;

    // Wait for the child thread.
    long ret = NULL;
    pthread_join(mPriv->thrd, (void**)&ret);
    LSLOG("child thread exit with return value %d", (int)ret);

    // Finish the saver.
    if (mSaver) {
        mSaver->finish();
    }

    // Release the resource.
    if (mPriv->buf) free(mPriv->buf);

    LSLOG("exit");
    return (int)ret;
}

// override
void KLogger::stop() {
    LSLOG("Stop the child thread.");
    pthread_kill(mPriv->thrd, SIGUSR1);
}
