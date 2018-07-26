#ifndef FILESAVERS_H
#define FILESAVERS_H

#include <string>

#include "LogSaver.h"

using namespace std;
using namespace logsaver;

namespace logsaver {

/**
 * The StdoutSaver class saves buffers to stdout.
 */
class StdoutSaver : public FileSaver {
public:
    int prepare(); // override
    int save(const char *buf, int bytes); // override
    void finish(); // override
};

/**
 * Save nothing, it is a black hole.
 */
class DummyFileSaver : public FileSaver {
public:
    int prepare(); // override
    int save(const char *buf, int bytes); // override
    void finish(); // override
};

/**
 * Save directly to the given filepath.
 */
class SimpleFileSaver : public FileSaver {
public:
    SimpleFileSaver(string filepath);
    int prepare(); // override
    int save(const char *buf, int bytes); // override
    void finish(); // override
protected:
    string mFilePath;
    FILE * mFile;
};

/**
 * Save to filepath_<suffix>.
 * Swap file when its size reach the max limit.
 */
class SuffixedFileSaverBase : public FileSaver {
public:
    SuffixedFileSaverBase(string filePathBase);
    int prepare(); // override
    int save(const char *buf, int bytes); // override
    void finish(); // override
protected:
    virtual string getNextSuffix() = 0;
protected:
    string mFilePathBase;
};

/**
 * Use an incremental index as suffix to SuffixedFileSaverBase.
 * The index is read from and saved in a normal file.
 */
class IndexedFileSaver : public SuffixedFileSaverBase {
public:
    IndexedFileSaver(string filePathBase);
protected:
    string getNextSuffix(); // override
};

/**
 * Same like IndexedFileSaver.
 * But read and save the incremental index to a presist property.
 */
class PropertiedFileSaver : public SuffixedFileSaverBase {
public:
    PropertiedFileSaver(string filePathBase);
protected:
    string getNextSuffix(); // override
};

/**
 * Use the create date-time as suffix to SuffixedFileSaverBase.
 */
class DatedFileSaver : public SuffixedFileSaverBase {
public:
    DatedFileSaver(string filePathBase);
protected:
    string getNextSuffix(); // override
};

} // namespace logsaver

#endif //FILESAVERS_H
