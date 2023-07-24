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

    void debug(const std::string &text, const char *date, const char *time, const std::string &file,
               const std::string &func, const int line);
    void info(const std::string &text, const char *date, const char *time, const std::string &file,
              const std::string &func, const int line);
    void warning(const std::string &text, const char *date, const char *time, const std::string &file,
                 const std::string &func, const int line);
    void error(const std::string &text, const char *date, const char *time, const std::string &file,
               const std::string &func, const int line);
    void fatal(const std::string &text, const char *date, const char *time, const std::string &file,
               const std::string &func, const int line);

private:
    LogTarget target = LogTarget::terminal;
    LogLevel level = LogLevel::debug;
    std::string path = "";  // log file path
    std::ofstream outFile;

    bool isAsync = false;

    std::mutex mtx;

    std::unique_ptr<BlockQueue<std::string>> blockQueue = nullptr;
    std::unique_ptr<std::thread> writeThread = nullptr;

private:
    TinyLogger() = default;
    ~TinyLogger();

    TinyLogger(const TinyLogger &rhs) = delete;
    TinyLogger(TinyLogger &&rhs) = delete;
    TinyLogger &operator=(const TinyLogger &rhs) = delete;
    TinyLogger &operator=(TinyLogger &&rhs) = delete;

    void write(const std::string &text, const LogLevel &level, const char *date, const char *time,
               const std::string &file, const std::string &func, const int line);
    void asyncOutput();
};

#define LOG_DEBUG(text) TinyLogger::getInstance()->debug(text, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);
#define LOG_INFO(text) TinyLogger::getInstance()->info(text, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);
#define LOG_WARNING(text) \
    TinyLogger::getInstance()->warning(text, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);
#define LOG_ERROR(text) TinyLogger::getInstance()->info(text, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);
#define LOG_FATAL(text) TinyLogger::getInstance()->info(text, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);

#endif
