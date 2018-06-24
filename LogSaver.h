#ifndef LOGSAVER_H
#define LOGSAVER_H

class LogCfg {
public:
    LogCfg();
    void showUsage();
    bool parse(int argc, char **argv);
};

class LogSaver {
public:
    LogSaver(LogCfg &cfg);
    int run();
};

#endif //LOGSAVER_H
