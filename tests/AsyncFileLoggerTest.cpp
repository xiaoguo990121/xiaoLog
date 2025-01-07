#include <xiaoLog/Logger.h>
#include <xiaoLog/AsyncFileLogger.h>

int main()
{
    xiaoLog::AsyncFileLogger asyncFileLogger;
    asyncFileLogger.setFileName("async_test");
    asyncFileLogger.startLogging();

    xiaoLog::Logger::setOutputFunction(
        [&](const char *msg, const uint64_t len)
        {
            asyncFileLogger.output(msg, len);
        },
        [&]()
        { asyncFileLogger.flush(); });

    asyncFileLogger.setFileSizeLimit(10000000);

    int i = 0;
    while (i < 1000000)
    {
        ++i;
        if (i % 100 == 0)
        {
            LOG_ERROR << "this is the " << i << "the log";
            continue;
        }
        LOG_INFO << "this is the " << i << "th log";
        ++i;
        LOG_DEBUG << "this is the " << i << "th log";
    }
}