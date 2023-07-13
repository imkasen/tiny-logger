#include "tiny_logger.h"
#include <ctime>
#include <iomanip>
#include <iostream>

using std::cout;
using std::string;

TinyLogger::TinyLogger()
{
    this->target = LogTarget::terminal;
    this->level = LogLevel::debug;
    const string &msg = "[Tiny Logger] " + static_cast<string>(__FILE__) + " " + TinyLogger::getCurrentTime() +
                        " : ==== Start logging ====\n";
    cout << msg;
}

TinyLogger::~TinyLogger()
{
    const string &msg =
        "[Tiny Logger] " + static_cast<string>(__FILE__) + " " + this->getCurrentTime() + " : ==== End logging ====\n";
    if (this->target != LogTarget::terminal && this->outFile.is_open())
    {
        this->outFile << msg;
        this->outFile.flush();
        this->outFile.close();
    }
    if (this->target != LogTarget::file)
    {
        cout << msg;
    }
}

TinyLogger &TinyLogger::getInstance()
{
    static TinyLogger instance;
    return instance;
}

void TinyLogger::init(const LogTarget &target, const LogLevel &level, const std::string &path)
{
    this->target = target;
    this->level = level;
    this->path = path;
    /*
    if (this->target != LogTarget::terminal)
    {
        this->outFile.open(this->path, std::ios::out | std::ios::app);
        this->outFile << init_msg;
    }
    if (this->target != LogTarget::file)
    {
        std::cout << init_msg;
    }
    */
}

string TinyLogger::getCurrentTime()
{
    std::time_t now_t = time(nullptr);
    struct tm now_s;
#ifdef _WIN32
    localtime_s(&now_s, &now_t);
#else
    localtime_r(&now_t, &now_s);
#endif
    return std::to_string(now_s.tm_year + 1900) + "-" + std::to_string(now_s.tm_mon + 1) + "-" +
           std::to_string(now_s.tm_mday) + " " + std::to_string(now_s.tm_hour) + ":" + std::to_string(now_s.tm_min) +
           ":" + std::to_string(now_s.tm_sec);
}

void TinyLogger::output(const std::string &text, const LogLevel &level)
{
    string msg;
    switch (level)
    {
        case LogLevel::debug:
            msg = "[DEBUG] ";
            break;
        case LogLevel::info:
            msg = "[INFO] ";
            break;
        case LogLevel::warning:
            msg = "[WARNING] ";
            break;
        case LogLevel::error:
            msg = "[ERROR] ";
            break;
        default:
            msg = "";
            break;
    }
    msg += static_cast<string>(__FILE__) + " " + this->getCurrentTime() + " : " + text + "\n";
    if (this->level <= level && this->target != LogTarget::file)
    {
        std::cout << msg;
    }
    if (this->target != LogTarget::terminal)
    {
        this->outFile << msg;
    }
}

void TinyLogger::DEBUG(const std::string &text)
{
    this->output(text, LogLevel::debug);
}

void TinyLogger::INFO(const std::string &text)
{
    this->output(text, LogLevel::info);
}

void TinyLogger::WARNING(const std::string &text)
{
    this->output(text, LogLevel::warning);
}

void TinyLogger::ERROR(const std::string &text)
{
    this->output(text, LogLevel::error);
}
