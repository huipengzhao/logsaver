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
    "    logsaver -k -p /dev/kmsg -s 2m -t 600 -x persist.logsaver.k.idx /data/bootchart/klog" NL
    "        This command saves kernel logs into /data/bootchart/klog_YYMMDD-hhmmss," NL
    "        where <idx> is from the persist.logsaver.k.idx property." NL
    "        The output logs are splited into files based on size of 2MB." NL
    "        And the process will stop after 600 seconds." NL
    NL
    "    logsaver -a -p '-s ActivityManager:V Zygote:V' -s 4m -x date /data/bootchart/alog" NL
    "        This command saves android logs into /data/bootchart/alog_<date>," NL
    "        Use logcat parameter '-s ActivityManager:V Zygote:V' to filter the logs." NL
    "        The logsaver will split log files by 4MB, and it will never stop." NL
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

