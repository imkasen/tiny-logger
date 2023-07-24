#include "tiny_logger.h"
#include <ctime>
#include <iomanip>
#include <iostream>

using std::string;

// TODO: async, promise and future, ...
// TODO: comments
// TODO: config mingw，C++11, 14

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
        writeThread = std::make_unique<std::thread>(flushLogThread);
    }
}

/*
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
*/

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

void TinyLogger::write(const std::string &text, const LogLevel &level, const char *date, const char *time,
                       const string &file, const string &func, const int line)
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
    // TODO: ms, μs
    msg += static_cast<string>(date) + " " + static_cast<string>(time) + " - " + file + " - " + func + "() - Line " +
           std::to_string(line) + ": " + text + "\n";

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

void TinyLogger::DEBUG(const std::string &text)
{
    this->write(text, LogLevel::debug, __DATE__, __TIME__, __FILE__, __FUNCTION__, __LINE__);
}

/*
void TinyLogger::INFO(const std::string &text)
{
    this->write(text, LogLevel::info);
}

void TinyLogger::WARNING(const std::string &text)
{
    this->write(text, LogLevel::warning);
}

void TinyLogger::ERROR(const std::string &text)
{
    this->write(text, LogLevel::error);
}

void TinyLogger::FATAL(const std::string &text)
{
    this->write(text, LogLevel::fatal);
}
*/
