#include <stdio.h>

#include "LogSaver.h"

#define NL "\n"

LogCfg::LogCfg() {
}

void LogCfg::showUsage() {
    printf(
    "USAGE:" NL
    "    logsaver -k [options] file_out" NL
    "    logsaver -a [options] file_out" NL
    NL
    "OPTIONS:" NL
    "    -a" NL
    "        Save Android logs." NL
    NL
    "    -k" NL
    "        Save Kernel logs." NL
    NL
    "    -p parameters" NL
    "        In -a case, it is the same as logcat parameters." NL
    "        In -k case, it indicates the kmsg source." NL
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
    "        Type <index>:" NL
    "            <index> is a file path which contains a number as suffix." NL
    "            Logsaver will read it, increase it and save it back." NL
    "            After suffixed, logfile is like /data/bootchart/klog_XYZ" NL
    "        Type <property>:" NL
    "            <property> is a Android property which contains a number." NL
    "            Logsaver will getprop it, increase it and setprop it back." NL
    "            After suffixed, logfile is like /data/bootchart/klog_XYZ" NL
    "        Type date:" NL
    "            Use current date and time as suffix." NL
    "            After suffixed, logfile is like /data/bootchart/klog_YYMMDD-hhmmss" NL
    "        Unset means always write to the same file path." NL
    NL
    );
}

bool LogCfg::parse(int argc, char **argv) {
    return false;
}

LogSaver::LogSaver(LogCfg &cfg) {
}

int LogSaver::run() {
    return 0;
}

