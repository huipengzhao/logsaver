#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define LS_DEBUG
#include "utils.h"
#include "LogCfg.h"

using namespace logsaver;

#define NL "\n"
#define LOGOPTS "ahkp:s:t:ux:o:"

// return < 0 means something is wrong.
static long get_size(const char *str) {
    char *end = NULL;
    long n = strtol(str, &end, 0);
    if (end == NULL) return -1; // should not happen.
    if (end == str ) return -1; // no match pattern.
    int len = strlen(end);
    if (len == 1) { // support K/k/M/m.
        switch (*end) {
        case 'K': case 'k': n *= 1024;
            break;
        case 'M': case 'm': n *= 1024*1024;
            break;
        default:            n = -1;
            break;
        }
    } else
    if (len != 0) { // invalid pattern.
        return -1;
    }
    return n;
}

static long get_timeout(const char *str) {
    char *end = NULL;
    long n = strtol(str, &end, 0);
    if (end == NULL) return -1; // should not happen.
    if (end == str ) return -1; // no match pattern.
    if (*end != 0  ) return -1; // not only number.
    return n;
}

void LogCfg::showUsage() {
    L_RAW( NL
    "USAGE:" NL
    "    logsaver -h" NL
    "    logsaver -k [options] [-o file_out]" NL
    "    logsaver -a [options] [-o file_out]" NL
    "    logsaver -u [options] [-o file_out]" NL
    NL
    "    -k -a and -u are exclusive, the latter override the former." NL
    NL
    "OPTIONS:" NL
    "    -h" NL
    "        Show this manual." NL
    NL
    "    -a" NL
    "        Save Android logs." NL
    NL
    "    -k" NL
    "        Save Kernel logs." NL
    NL
    "    -u" NL
    "        Save UEvent logs." NL
    NL
    "    -o file_out" NL
    "        Output file path." NL
    NL
    "    -p parameters" NL
    "        In -a case, it is the same as logcat parameters." NL
    "        In -k case, it is ignored." NL
    "        In -u case, it is ignored." NL
    NL
    "    -s size" NL
    "        In byte. Max size of a single log file." NL
    "        When current log file reach the max size," NL
    "        the logsaver close it and open a new one to write logs." NL
    "        Often use with the -x option." NL
    NL
    "    -t timeout" NL
    "        In second. Quit after timeout. Unset means never quit." NL
    NL
    "    -x suffix" NL
    "        Indicates suffix type. 3 types: index, property and date." NL
    "        Suppose file_out is /data/bootchart/klog." NL
    NL
    "        Type <index>:" NL
    "            <index> is a file path which contains a number as suffix." NL
    "            Such as /data/bootchart/k_idx" NL
    "            Logsaver will read it, increase it and save it back." NL
    "            After suffixed, logfile is like /data/bootchart/klog_XYZ" NL
    NL
    "        Type <property>:" NL
    "            <property> is a persist property which contains a number." NL
    "            Such as persist.logsaver.a.idx" NL
    "            Logsaver will getprop it, increase it and setprop it back." NL
    "            After suffixed, logfile is like /data/bootchart/klog_XYZ" NL
    NL
    "        Type date:" NL
    "            The suffix is the date and time to create the log file." NL
    "            After suffixed, logfile is like /data/bootchart/klog_YYMMDD-hhmmss" NL
    NL
    "        Unset means always write to the same file path." NL
    NL
    "EXAMPLES:" NL
    "    logsaver -k -s 2m -t 600 -x persist.logsaver.k.idx -o /data/bootchart/klog" NL
    "        This command saves kernel logs into /data/bootchart/klog_YYMMDD-hhmmss," NL
    "        where <idx> is from the persist.logsaver.k.idx property." NL
    "        The output logs are splited into files based on size of 2MB." NL
    "        And the process will stop after 600 seconds." NL
    NL
    "    logsaver -a -p '-s ActivityManager:V Zygote:V' -s 4m -x date -o /data/bootchart/alog" NL
    "        This command saves android logs into /data/bootchart/alog_<date>," NL
    "        Use logcat parameter '-s ActivityManager:V Zygote:V' to filter the logs." NL
    "        The logsaver will split log files by 4MB, and it will never stop." NL
    NL
    );
}

LogCfg::LogCfg() {
    mLogType    = LOGTYPE_NONE;
    mParam      = "";
    mMaxSize    = -1;
    mTimeout    = -1;
    mSuffix     = "";
    mSuffixType = SFXTYPE_NONE;
    mFilePath   = "";
}

bool LogCfg::parse(int argc, char **argv) {
    // optind -- (int) the current index in *argv[].
    // optopt -- (int) the opt char.
    // optarg -- (char*) the opt argument string.
    int ch;
    while ((ch = getopt(argc, argv, LOGOPTS)) != -1) {
        switch (ch) {
        case 'h':
            mLogType = LOGTYPE_HELP;
            return true;
            break;
        case 'a':
            mLogType = LOGTYPE_ALOG;
            break;
        case 'k':
            mLogType = LOGTYPE_KMSG;
            break;
        case 'u':
            mLogType = LOGTYPE_UEVT;
            break;
        case 'o':
            mFilePath = optarg;
            break;
        case 'p':
            mParam = optarg;
            break;
        case 's': // should support K/k/M/m/G/g
            mMaxSize = (int)get_size(optarg);
            if (mMaxSize < 0) {
                L_RAW("Invalid argument for option -%c: %s\n", (char)ch, optarg);
                return false;
            }
            break;
        case 't':
            mTimeout = (int)get_timeout(optarg);
            if (mTimeout < 0) {
                L_RAW("Invalid argument for option -%c: %s\n", (char)ch, optarg);
                return false;
            }
            break;
        case 'x':
            mSuffix = optarg;
            if (mSuffix.compare("date") == 0) {
                mSuffixType = SFXTYPE_DATE;
            } else
            if (mSuffix.compare(0, strlen("persist."), "persist.") == 0) {
                // getprop, increase, and setprop back.
                mSuffixType = SFXTYPE_PROP;
            } else {
                // touch the mSuffix file.
                close(open(mSuffix.c_str(), O_CREAT|O_RDWR, 0600));
                if (access(mSuffix.c_str(), R_OK|W_OK) == 0) {
                    // read, increase, and write back.
                    mSuffixType = SFXTYPE_INDEX;
                } else {
                    L("Tried to touch file %s, but failed.\n", mSuffix.c_str());
                }
            }
            if (mSuffixType == SFXTYPE_NONE) {
                L_RAW("Invalid argument for option -%c: %s\n", (char)ch, mSuffix.c_str());
                return false;
            }
            break;
        case '?': // invalid option
            L_RAW("Invalid option -%c\n", (char)optopt);
            return false;
            break;
        case ':': // lack of param
            L_RAW("Lack of argument for option -%c\n", (char)optopt);
            return false;
            break;
        default:
            L_RAW("Unknown option -%c\n", (char)ch);
            return false;
            break;
        }
    }

    if (mLogType == LOGTYPE_NONE) {
        L_RAW("Must set -h, -k -a or -u option.\n");
        return false;
    }

    if (optind < argc) {
        L_RAW("Invalid argument: %s\n", argv[optind]);
        return false;
    }

    return true;
}

void LogCfg::show() {
    L_RAW("mLogType   : %d\n", mLogType);
    L_RAW("mParam     : %s\n", mParam.c_str());
    L_RAW("mMaxSize   : %d\n", mMaxSize);
    L_RAW("mTimeout   : %d\n", mTimeout);
    L_RAW("mSuffix    : %s\n", mSuffix.c_str());
    L_RAW("mSuffixType: %d\n", mSuffixType);
    L_RAW("mFilePath  : %s\n", mFilePath.c_str());
    fflush(stdout);
}

