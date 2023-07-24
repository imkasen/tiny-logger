#include "tiny_logger.h"

int main()
{
    auto logger = TinyLogger::getInstance();
    logger->init(TinyLogger::LogTarget::file_and_terminal, TinyLogger::LogLevel::debug, "./log", 1024);

    LOG_DEBUG("This is a debug message.");
    LOG_INFO("This is a info message.");
    LOG_WARNING("This is a warning message.");
    LOG_ERROR("This is a error message.");
    LOG_FATAL("This is a fatal message.");

    return 0;
}
