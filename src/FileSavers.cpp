#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"
#include "FileSavers.h"

using namespace logsaver;

//-----------------------------------------------------------------------------
// StdoutSaver
//-----------------------------------------------------------------------------
int StdoutSaver::prepare() { return 0; }
int StdoutSaver::save(const char *buf, int bytes) {
    return write(fileno(stdout), buf, bytes);
}
void StdoutSaver::finish() {}

//-----------------------------------------------------------------------------
// DummyFileSaver
//-----------------------------------------------------------------------------
int DummyFileSaver::prepare() { return 0; }
int DummyFileSaver::save(const char *buf, int bytes) { return bytes; }
void DummyFileSaver::finish() {}

//-----------------------------------------------------------------------------
// SimpleFileSaver
//-----------------------------------------------------------------------------
SimpleFileSaver::SimpleFileSaver(string filePath)
    : mFilePath(filePath), mFile(NULL) {}
int SimpleFileSaver::prepare() {
    mFile = fopen(mFilePath.c_str(), "w+");
    if (!mFile) { // open failed.
        LSLOG("Open file %s failed. Error: %s", mFilePath.c_str(), strerror(errno));
        return -errno;
    }
    return 0;
}
int SimpleFileSaver::save(const char *buf, int bytes) {
    if (!mFile) return -ENOENT;
    int n = fwrite(buf, 1, bytes, mFile);
    if (n > 0) { fflush(mFile); } // flush data to file immediately.
    return n;
}
void SimpleFileSaver::finish() {
    if (mFile) { fclose(mFile); }
}

//-----------------------------------------------------------------------------
// SuffixedFileSaverBase
//-----------------------------------------------------------------------------
SuffixedFileSaverBase::SuffixedFileSaverBase(string filePathBase)
    : mFilePathBase(filePathBase) {}
int SuffixedFileSaverBase::prepare() { return 0; }
int SuffixedFileSaverBase::save(const char *buf, int bytes) { return bytes; }
void SuffixedFileSaverBase::finish() {}

//-----------------------------------------------------------------------------
// IndexedFileSaver
//-----------------------------------------------------------------------------
IndexedFileSaver::IndexedFileSaver(string filePathBase)
    : SuffixedFileSaverBase(filePathBase) {}
string IndexedFileSaver::getNextSuffix() { return "_sfx"; }

//-----------------------------------------------------------------------------
// PropertiedFileSaver
//-----------------------------------------------------------------------------
PropertiedFileSaver::PropertiedFileSaver(string filePathBase)
    : SuffixedFileSaverBase(filePathBase) {}
string PropertiedFileSaver::getNextSuffix() { return "_sfx"; }

//-----------------------------------------------------------------------------
// DatedFileSaver
//-----------------------------------------------------------------------------
DatedFileSaver::DatedFileSaver(string filePathBase)
    : SuffixedFileSaverBase(filePathBase) {}
string DatedFileSaver::getNextSuffix() { return "_sfx"; }

