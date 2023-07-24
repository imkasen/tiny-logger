#ifndef TINY_LOGGER_H
#define TINY_LOGGER_H

#include "block_queue.h"
#include <fstream>
#include <memory>
#include <string>
#include <thread>

class TinyLogger
{
public:
    enum class LogLevel
    {
        debug,
        info,
        warning,
        error,
        fatal
    };

    enum class LogTarget
    {
        file,
        terminal,
        file_and_terminal
    };

public:
    static TinyLogger *getInstance();
    void init(const LogTarget &target, const LogLevel &level, const std::string &path = "./log",
              const size_t maxQueueCapacity = 0);

    std::string getCurrentTime();
    void setLevel(const LogLevel &level);
    LogLevel getLevel() const;
    void setTarget(const LogTarget &target);
    LogTarget getTarget() const;

    static void flushLogThread();

    void DEBUG(const std::string &text);
    // void INFO(const std::string &text);
    // void WARNING(const std::string &text);
    // void ERROR(const std::string &text);
    // void FATAL(const std::string &text);

private:
    LogTarget target;
    LogLevel level;
    std::string path;  // log file path
    std::ofstream outFile;

    bool isAsync;

    std::mutex mtx;

    std::unique_ptr<BlockQueue<std::string>> blockQueue;
    std::unique_ptr<std::thread> writeThread;

private:
    TinyLogger();
    ~TinyLogger();

    TinyLogger(const TinyLogger &rhs) = delete;
    TinyLogger(TinyLogger &&rhs) = delete;
    TinyLogger &operator=(const TinyLogger &rhs) = delete;
    TinyLogger &operator=(TinyLogger &&rhs) = delete;

    void write(const std::string &text, const LogLevel &level, const char *date, const char *time,
               const std::string &file, const std::string &func, const int line);
    void asyncOutput();
};

#endif
