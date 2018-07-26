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
#include "FileSavers.h"

using namespace logsaver;

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

} //namespace logsaver

int LogSaverPriv::run(LogCfg cfg) {
    cfg.show();

    // Determine the file saver.
    if (cfg.mFilePath.empty()) {
        mFSaver = new StdoutSaver();
    } else if (cfg.mSuffixType == LogCfg::SfxType::SFXTYPE_NONE) {
        mFSaver = new SimpleFileSaver(cfg.mFilePath);
    } else if (cfg.mSuffixType == LogCfg::SfxType::SFXTYPE_INDEX) {
        mFSaver = new IndexedFileSaver(cfg.mFilePath, cfg.mSuffix);
    } else if (cfg.mSuffixType == LogCfg::SfxType::SFXTYPE_PROP) {
        mFSaver = new PropertiedFileSaver(cfg.mFilePath);
    } else if (cfg.mSuffixType == LogCfg::SfxType::SFXTYPE_DATE) {
        mFSaver = new DatedFileSaver(cfg.mFilePath);
    } else { /* Never run here */ }

    // Determine the logger.
    switch ((int)cfg.mLogType) {
    case LogCfg::LogType::LOGTYPE_KMSG:
        mLogger = new KLogger();
        break;
    case LogCfg::LogType::LOGTYPE_ALOG:
        //mLogger = new ALogger();
        break;
    case LogCfg::LogType::LOGTYPE_UEVT:
        //mLogger = new ULogger();
        break;
    }

    // Link the logger with the saver.
    mLogger->setFileSaver(mFSaver);

    // Register signal handler, for SIGINT and SIGALRM.
    register_sighandler(this);

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

