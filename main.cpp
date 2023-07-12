#include "tiny_logger.h"

int main()
{
    TinyLogger logger(TinyLogger::LogTarget::file_and_terminal, TinyLogger::LogLevel::debug, "log.log");
    logger.DEBUG("debug");
    logger.ERROR("error");
    logger.INFO("info");
    logger.WARNING("warning");

    return 0;
}
