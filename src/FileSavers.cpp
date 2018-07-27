#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LS_DEBUG
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
    : mFilePathBase(filePathBase), mCurrFile(NULL) {}

int SuffixedFileSaverBase::prepare() {
    mCurrFile = openNextFile();
    return (mCurrFile!=NULL ? 0 : -ENOENT);
}

int SuffixedFileSaverBase::save(const char *buf, int bytes) {
    if (!mCurrFile) return -ENOENT;
    int n = fwrite(buf, 1, bytes, mCurrFile);
    if (n > 0) { fflush(mCurrFile); } // flush data to file immediately.
    return n;
}

void SuffixedFileSaverBase::finish() {
    if (mCurrFile) { fclose(mCurrFile); }
}

FILE *SuffixedFileSaverBase::openNextFile() {
    string sfx = getNextSuffix();
    string fpath = mFilePathBase + "_" + sfx;
    FILE *fp = fopen(fpath.c_str(), "w+");
    if (!fp) { // open failed.
        LSLOG("Open file %s failed. Error: %s", fpath.c_str(), strerror(errno));
    }
    return fp;
}

//-----------------------------------------------------------------------------
// IndexedFileSaver
//-----------------------------------------------------------------------------
IndexedFileSaver::IndexedFileSaver(string filePathBase, string indexFilePath)
    : SuffixedFileSaverBase(filePathBase), mIndexPath(indexFilePath) {}

string IndexedFileSaver::getNextSuffix() {
    // Now open the index file.
    FILE *fp = fopen(mIndexPath.c_str(), "r+");
    if (!fp) {
        LSLOG("Open index file %s failed. Error: %s", mIndexPath.c_str(), strerror(errno));
        return "badsfx";
    }

    // Read a number from the file.
    int n = 0;
    char buf[8] = {0};
    if ((n = fread(buf, 1, 7, fp)) < 0) {
        LSLOG("Read from file %s failed. Error: %s", mIndexPath.c_str(), strerror(errno));
        fclose(fp);
        return "badsfx";
    }
    buf[n] = 0;

    // Keep the number within 0~999.
    int num = strtol(buf, NULL, 10);
    num %= 1000;
    sprintf(buf, "%03u", num);
    string ret(buf);

    // Increase and write it back.
    num += 1;
    num %= 1000;
    n = sprintf(buf, "%03u", num);
    rewind(fp);
    fwrite(buf, 1, n+1, fp); // Here we do not care error.
    fclose(fp);

    return ret;
}

//-----------------------------------------------------------------------------
// PropertiedFileSaver
//-----------------------------------------------------------------------------
PropertiedFileSaver::PropertiedFileSaver(string filePathBase)
    : SuffixedFileSaverBase(filePathBase) {}
string PropertiedFileSaver::getNextSuffix() { return "sfx"; }

//-----------------------------------------------------------------------------
// DatedFileSaver
//-----------------------------------------------------------------------------
DatedFileSaver::DatedFileSaver(string filePathBase)
    : SuffixedFileSaverBase(filePathBase) {}
string DatedFileSaver::getNextSuffix() { return "sfx"; }

