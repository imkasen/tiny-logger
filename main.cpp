#include "tiny_logger.h"

int main()
{
    auto logger = TinyLogger::getInstance();
    logger->init(TinyLogger::LogTarget::file_and_terminal, TinyLogger::LogLevel::debug, "./log", 1024);

    logger->DEBUG("debug");
    logger->INFO("info");
    logger->WARNING("warning");
    logger->ERROR("error");
    logger->FATAL("fatal");

    return 0;
}
