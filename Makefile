
SRC :=
SRC += src/LogCfg.cpp
SRC += src/LogSaver.cpp
SRC += src/KLogger.cpp
SRC += src/main.cpp

logsaver: $(SRC)
	g++ -o $@ $^ -Wall -std=c++11 -lpthread

clean:
	rm -f logsaver