#ifndef TINY_LOGGER_H
#define TINY_LOGGER_H

#include <fstream>
#include <string>

class TinyLogger
{
public:
    enum class LogLevel  // 日志等级
    {
        debug,
        info,
        warning,
        error
    };

    enum class LogTarget  // 日志输出目标
    {
        file,
        terminal,
        file_and_terminal
    };

public:
    TinyLogger();
    TinyLogger(const LogTarget &target, const LogLevel &level, const std::string &path);
    ~TinyLogger();

    static std::string getCurrentTime();
    void DEBUG(const std::string &text);
    void INFO(const std::string &text);
    void WARNING(const std::string &text);
    void ERROR(const std::string &text);

private:
    LogTarget target;
    LogLevel level;
    std::string path;  // 日志文件路径
    std::ofstream outFile;
    void output(const std::string &text, const LogLevel &level);  // 输出行为
};

#endif
