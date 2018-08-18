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
#include "ULogger.h"
#include "FileSavers.h"

namespace logsaver {

/**
 * The LogSaverPriv class does the real job.
 */
class LogSaverPriv {
public:
    int run(LogCfg cfg);

public:
    FileSaver *mFSaver;
    Logger    *mLogger;

private:
    FileSaver *createFileSaver(LogCfg cfg);
    Logger *createLogger(LogCfg cfg);
};

} //namespace logsaver

using namespace logsaver;

static void logsaver_sighandler(int sig) {
    LogSaverPriv *p = (LogSaverPriv*)get_specific();
    if (!p || !p->mLogger) {
        // should not happen, we just cry for it.
        L("Error: LogSaverPriv or LogSaverPriv.mLogger should not be NULL!");
        return;
    }
    if(sig==SIGINT){
        L("Ctrl + C pressed, quit.");
        p->mLogger->stop();
    }
    if (sig==SIGALRM) {
        L("The logging is timeout.");
        p->mLogger->stop();
    }
}

static void register_sighandler(void *ptr) {
    // save *this* ptr to thread specific area, so we can get use of it
    // in the signal handler of this thread.
    // Yes, we can also save the ptr in the static data section, that way
    // is easier, but using a global variable to save the ptr makes the
    // LogSaver have to be singleton.
    set_specific(ptr);

    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = logsaver_sighandler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGALRM, &action, NULL);
}

FileSaver *LogSaverPriv::createFileSaver(LogCfg cfg) {
    FileSaver * fsaver = NULL;
    if (cfg.mFilePath.empty()) {
        fsaver = new StdoutSaver();
    } else if (cfg.mSuffixType == LogCfg::SFXTYPE_NONE) {
        fsaver = new SimpleFileSaver(cfg.mFilePath);
    } else if (cfg.mSuffixType == LogCfg::SFXTYPE_INDEX) {
        fsaver = new IndexedFileSaver(cfg.mFilePath, cfg.mSuffix);
    } else if (cfg.mSuffixType == LogCfg::SFXTYPE_PROP) {
        fsaver = new PropertiedFileSaver(cfg.mFilePath);
    } else if (cfg.mSuffixType == LogCfg::SFXTYPE_DATE) {
        fsaver = new DatedFileSaver(cfg.mFilePath);
    } else { /* Never run here */ }
    return fsaver;
}

Logger *LogSaverPriv::createLogger(LogCfg cfg) {
    Logger * logger = NULL;
    switch ((int)cfg.mLogType) {
    case LogCfg::LOGTYPE_KMSG:
        logger = new KLogger();
        break;
    case LogCfg::LOGTYPE_ALOG:
        //logger = new ALogger();
        break;
    case LogCfg::LOGTYPE_UEVT:
        logger = new ULogger();
        break;
    default:
        break; /* Never run here */
    }
    return logger;
}

int LogSaverPriv::run(LogCfg cfg) {
    L("LogSaver start.");
    cfg.show();

    // Determine the file saver.
    mFSaver = createFileSaver(cfg);

    // Determine the logger.
    mLogger = createLogger(cfg);

    // Link the logger with the saver.
    mLogger->setFileSaver(mFSaver);

    // Register signal handler, for SIGINT and SIGALRM.
    register_sighandler(this);

    // Start timer on demand.
    if (cfg.mTimeout > 0) {
        set_oneshot_timer(cfg.mTimeout*1000);
    }

    // Go get the logs.
    int err = 0;
    err = mFSaver->prepare();
    if (err) {
        L("FileSaver prepare() failed. err=%d", err);
    } else {
        L("FileSaver prepared.");
        
        err = mLogger->go();
        L("Logger stopped.");
        
        mFSaver->finish();
        L("FileSaver finished.");
    }

    // Stop timer if started.
    if (cfg.mTimeout > 0) {
        unset_oneshot_timer();
    }

    L("LogSaver exit. err=%d", err);
    return err;
}

/**
 * The LogSaver class is actually a wrapper of LogSaverPriv.
 */
LogSaver::LogSaver() : priv(new LogSaverPriv()) {}

int LogSaver::run(LogCfg cfg) {
    return priv->run(cfg);
}
