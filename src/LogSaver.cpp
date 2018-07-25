#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define LS_DEBUG
#include "utils.h"
#include "LogSaver.h"
#include "KLogger.h"

/**
 * The DefaultFileSaver class saves buffers to stdout.
 */
class DefaultFileSaver : public FileSaver {
public:
    // override
    int save(const char *buf, int bytes) {
        return write(fileno(stdout), buf, bytes);
    }
    // override
    void finish() {};
};

/**
 * The LogSaverPriv class does the real job.
 */
class LogSaverPriv {
public:
    int run(LogCfg cfg);

public:
    FileSaver *mFSaver;
    Logger    *mLogger;
};

static void logsaver_sighandler(int sig) {
    LogSaverPriv *p = (LogSaverPriv*)get_specific();
    if (!p || !p->mLogger) {
        // should not happen, we just cry for it.
        LSLOG("Error: LogSaverPriv or LogSaverPriv.mLogger should not be NULL!");
        return;
    }
    if(sig==SIGINT){
        LSLOG("Ctrl + C pressed, quit.");
        p->mLogger->stop();
    }
    if (sig==SIGALRM) {
        LSLOG("The logging is timeout.");
        p->mLogger->stop();
    }
}

static void register_sighandler() {
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = logsaver_sighandler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGALRM, &action, NULL);
}

int LogSaverPriv::run(LogCfg cfg) {
    set_specific(this);

    cfg.show();

    // Determine the file saver.
    if (cfg.mFilePath.empty()) {
        mFSaver = new DefaultFileSaver();
    }

    // Determine the logger.
    switch ((int)cfg.mLogType) {
    case LogCfg::LogType::LOGTYPE_KMSG:
        mLogger = new KLogger(mFSaver);
        break;
    case LogCfg::LogType::LOGTYPE_ALOG:
        //mLogger = new ALogger(mFSaver);
        break;
    case LogCfg::LogType::LOGTYPE_UEVT:
        //mLogger = new ULogger(mFSaver);
        break;
    }

    // Register signal handler, for SIGINT and SIGALRM.
    register_sighandler();

    // Start timer on demand.
    if (cfg.mTimeout > 0) {
        set_oneshot_timer(cfg.mTimeout*1000);
    }

    // Go get the logs.
    LSLOG("LogSaver start.");
    int err = mLogger->go();
    LSLOG("LogSaver exit.");

    // Stop timer if started.
    if (cfg.mTimeout > 0) {
        unset_oneshot_timer();
    }
    
    return err;
}

/**
 * The LogSaver class is actually a wrapper of LogSaverPriv.
 */
LogSaver::LogSaver() : priv(new LogSaverPriv()) {}

int LogSaver::run(LogCfg cfg) {
    return priv->run(cfg);
}

