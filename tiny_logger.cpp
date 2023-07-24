#include "tiny_logger.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

using std::string;

TinyLogger::~TinyLogger()
{
    if (this->writeThread && this->writeThread->joinable())
    {
        while (!this->blockQueue->empty())
        {
            this->blockQueue->flush();
        }
        this->blockQueue->close();
        this->writeThread->join();
    }

    if (this->outFile.is_open())
    {
        std::lock_guard<std::mutex> lck(mtx);
        this->outFile.flush();
        this->outFile << "\n";
        this->outFile.close();
    }
}

TinyLogger *TinyLogger::getInstance()
{
    static TinyLogger instance;
    return &instance;
}

void TinyLogger::init(const LogTarget &target, const LogLevel &level, const std::string &path,
                      const size_t maxQueueCapacity)
{
    this->target = target;
    this->level = level;
    this->path = path;

    if (this->target != LogTarget::terminal)
    {
        std::lock_guard<std::mutex> lck(mtx);
        this->outFile.open(this->path, std::ios::out | std::ios::app);
        if (!this->outFile)
        {
            std::cerr << "Error opening log file: " << this->path << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if (maxQueueCapacity > 1)  //  maxCapacity > 1: async, maxCapacity == 1: sync
    {
        this->isAsync = true;
        blockQueue = std::make_unique<BlockQueue<string>>(maxQueueCapacity);
        // create one single thread to read log messages constantly from the block queue and write them into log file
        writeThread = std::make_unique<std::thread>(flushLogThread);
    }
}

string TinyLogger::getCurrentTime()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000000;
    struct tm now_s;

#ifdef _WIN32
    localtime_s(&now_s, &now_t);
#elif defined(__linux__) || defined(__unix__)
    localtime_r(&now_t, &now_s);
#else
#error "Unknown Platform"
#endif

    // [level] Year-Month-Day hours:minutes:seconds.microseconds - file_name - func_name - line_number: messages.
    return std::to_string(now_s.tm_year + 1900) + "-" + std::to_string(now_s.tm_mon + 1) + "-" +
           std::to_string(now_s.tm_mday) + " " + std::to_string(now_s.tm_hour) + ":" + std::to_string(now_s.tm_min) +
           ":" + std::to_string(now_s.tm_sec) + "." + std::to_string(us);
}

void TinyLogger::setLevel(const LogLevel &level)
{
    this->level = level;
}

TinyLogger::LogLevel TinyLogger::getLevel() const
{
    return this->level;
}

void TinyLogger::setTarget(const LogTarget &target)
{
    this->target = target;
}

TinyLogger::LogTarget TinyLogger::getTarget() const
{
    return this->target;
}

void TinyLogger::flushLogThread()
{
    TinyLogger::getInstance()->asyncOutput();
}

void TinyLogger::asyncOutput()
{
    string msg = "";
    while (this->blockQueue->pop(msg))
    {
        std::lock_guard<std::mutex> lck(mtx);
        if (this->target != LogTarget::file)
        {
            std::cout << msg;
        }
        if (this->target != LogTarget::terminal)
        {
            this->outFile << msg;
        }
    }
}

void TinyLogger::write(const std::string &text, const LogLevel &level, const string &file, const string &func,
                       const int line)
{
    std::lock_guard<std::mutex> lck(mtx);

    // organize output message
    string msg = "";
    switch (level)
    {
        case LogLevel::debug:
            msg = "[DEBUG]   ";
            break;
        case LogLevel::info:
            msg = "[INFO]    ";
            break;
        case LogLevel::warning:
            msg = "[WARNING] ";
            break;
        case LogLevel::error:
            msg = "[ERROR]   ";
            break;
        case LogLevel::fatal:
            msg = "[FATAL]   ";
            break;
        default:
            msg = "[UNKNOWN] ";
            break;
    }
    msg += TinyLogger::getCurrentTime() + " - " + file + " - " + func + "() - Line " + std::to_string(line) + ": " +
           text + "\n";

    if (level >= this->level)
    {
        if (this->isAsync && this->blockQueue && !this->blockQueue->full())
        {
            this->blockQueue->push(msg);
        }
        else  // sync
        {
            if (this->target != LogTarget::file)
            {
                std::cout << msg;
            }
            if (this->target != LogTarget::terminal)
            {
                this->outFile << msg;
            }
        }
    }
}

void TinyLogger::debug(const std::string &text, const std::string &file, const std::string &func, const int line)
{
    this->write(text, LogLevel::debug, file, func, line);
}

void TinyLogger::info(const std::string &text, const std::string &file, const std::string &func, const int line)
{
    this->write(text, LogLevel::info, file, func, line);
}

void TinyLogger::warning(const std::string &text, const std::string &file, const std::string &func, const int line)
{
    this->write(text, LogLevel::warning, file, func, line);
}

void TinyLogger::error(const std::string &text, const std::string &file, const std::string &func, const int line)
{
    this->write(text, LogLevel::error, file, func, line);
}

void TinyLogger::fatal(const std::string &text, const std::string &file, const std::string &func, const int line)
{
    this->write(text, LogLevel::fatal, file, func, line);
}
