#include <xiaoLog/Logger.h>

int main()
{
    int i;
    LOG_COMPACT_DEBUG << "Hello, world!";
    LOG_DEBUG << (float)3.14;
    LOG_DEBUG << (const char)'8';
    LOG_DEBUG << (long double)3.1415;
    LOG_DEBUG << &i;
    LOG_DEBUG << xiaoLog::Fmt("%.3g", 3.1415926);
    LOG_DEBUG << "debug log!" << 1;
    LOG_TRACE << "trace log!" << 2;
    LOG_INFO << "info log!" << 3;
    LOG_WARN << "warning log!" << 4;
}