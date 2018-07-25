#ifndef LOGCFG_H
#define LOGCFG_H

#include <string>

using namespace std;

class LogCfg {
public:
    enum LogType {
        LOGTYPE_NONE = 0,
        LOGTYPE_HELP,
        LOGTYPE_KMSG,
        LOGTYPE_ALOG,
        LOGTYPE_UEVT,
    };

    enum SfxType {
        SFXTYPE_NONE = 0,
        SFXTYPE_INDEX,
        SFXTYPE_PROP,
        SFXTYPE_DATE,
    };

    LogCfg();
    void showUsage();
    bool parse(int argc, char **argv);
    void show(); // only for debug

    LogType mLogType;
    string  mParam;
    int     mMaxSize;
    int     mTimeout;
    string  mSuffix;
    SfxType mSuffixType;
    string  mFilePath;
};

#endif //LOGCFG_H