#ifndef LOGSAVERBASE_H
#define LOGSAVERBASE_H

namespace logsaver {

/**
 * Abstract class to save buffers into files.
 */
class FileSaver {
public:
    FileSaver() {};
    virtual ~FileSaver() {};

    /**
     * Return 0 means success, others on failure.
     */
    virtual int prepare() = 0;

    /**
     * Like the syscall write().
     */
    virtual int save(const char* buf, int bytes) = 0;

    /**
     * Finalize the saver.
     */
    virtual void finish() = 0;
};

/**
 * Abstract class to produce log buffers.
 */
class Logger {
public:
    Logger() : mSaver(NULL) {}
    virtual ~Logger() {};

    /**
     * Start logging.
     * This function will block until stop() is called,
     * otherwise some error may occur.
     */
    virtual int go() = 0;

    /**
     * Stop logging.
     * This function will terminate the go() function.
     */
    virtual void stop() = 0;

    /**
     * Set a file saver. Should be called only once before caling go().
     */
    void setFileSaver(FileSaver *saver) {
        if (!mSaver) {
            mSaver = saver;
        }
    }

    /**
     * Get the file saver.
     */
    FileSaver *getFileSaver() { return mSaver; }

protected:
    FileSaver *mSaver;
};

} //namesapce logsaver

#endif //LOGSAVERBASE_H
