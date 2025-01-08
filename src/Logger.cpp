/**
 * @file Logger.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-04
 *
 *
 */
#include <xiaoLog/Logger.h>
#include <assert.h>
#include <thread>
#include <iostream>
#ifdef __unix__
#include <unistd.h>
#include <sys/syscall.h>
#include <sstream>
#else
#endif
#if (__cplusplus >= 201703L) || \
    (defined(_MSVC_LANG) &&     \
     (_MSVC_LANG >=             \
      201703L)) // c++17 - the _MSVC_LANG extra check can be
                // removed if the support for VS2015 & 2017 is dropped
#include <algorithm>
#else
namespace std
{
    inline xiaoLog::Logger::LogLevel clamp(xiaoLog::Logger::LogLevel v,
                                           xiaoLog::Logger::LogLevel min,
                                           xiaoLog::Logger::LogLevel max)
    {
        return (v < min) ? min : (v > max) ? max
                                           : v;
    }
} // namespace std
#endif
#if defined __FreeBSD__
#include <pthread_np.h>
#endif

namespace xiaoLog
{
    // helper class for known string length at compile time
    class T
    {
    public:
        T(const char *str, unsigned len) : str_(str), len_(len)
        {
            assert(strlen(str) == len_);
        }

        const char *str_;
        const unsigned len_;
    };

    const char *strerror_tl(int savedErrno)
    {
#ifndef _MSC_VER
        return strerror(savedErrno);
#else

#endif
    }

    inline LogStream &operator<<(LogStream &s, T v)
    {
        s.append(v.str_, v.len_);
        return s;
    }

    inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v)
    {
        s.append(v.data_, v.size_);
        return s;
    }
}

using namespace xiaoLog;

static thread_local uint64_t lastSecond_{0};
static thread_local char lastTimeString_[32] = {0};
#ifdef __linux__
static thread_local pid_t threadId_{0};
#else
static thread_local uint64_t threadId_{0};
#endif

void Logger::formatTime()
{
    uint64_t now = static_cast<uint64_t>(date_.secondsSinceEpoch());
    uint64_t microSec =
        static_cast<uint64_t>(date_.microSecondsSinceEpoch() -
                              date_.roundSecond().microSecondsSinceEpoch());

    if (now != lastSecond_)
    {
        lastSecond_ = now;
        if (displayLocalTime_())
        {
#ifndef _MSC_VER
            strncpy(lastTimeString_,
                    date_.toFormattedStringLocal(false).c_str(),
                    sizeof(lastTimeString_) - 1);
#else

#endif
        }
        else
        {
#ifndef _MSC_VER
            strncpy(lastTimeString_,
                    date_.toFormattedString(false).c_str(),
                    sizeof(lastTimeString_) - 1);
#else
#endif
        }
    }
    logStream_ << T(lastTimeString_, 17); // 添加时间
    char tmp[32];
    if (displayLocalTime_())
    {
        snprintf(tmp,
                 sizeof(tmp),
                 ".%06llu ",
                 static_cast<long long unsigned int>(microSec));
        logStream_ << T(tmp, 8);
    }
    else
    {
        snprintf(tmp,
                 sizeof(tmp),
                 ".%06llu UTC ",
                 static_cast<long long unsigned int>(microSec));
        logStream_ << T(tmp, 12);
    }
#ifdef __linux__
    if (threadId_ == 0)
        threadId_ = static_cast<pid_t>(::syscall(SYS_gettid));
#else
    if (threadId_ == 0)
    {
        pthread_threadid_np(NULL, &threadId_);
    }
#endif
    logStream_ << threadId_;
}
static const char *logLevelStr[Logger::LogLevel::kNumberOfLogLevels] = {
    " TRACE ",
    " DEBUG ",
    " INFO  ",
    " WARN  ",
    " ERROR ",
    " FATAL ",
};

Logger::Logger(SourceFile file, int line)
    : sourceFile_(file), fileLine_(line), level_(kInfo)
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
}
Logger::Logger(SourceFile file, int line, LogLevel level)
    : sourceFile_(file),
      fileLine_(line),
      level_(std::clamp(level, kTrace, kFatal))
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
}
Logger::Logger(SourceFile file, int line, LogLevel level, const char *func)
    : sourceFile_(file),
      fileLine_(line),
      level_(std::clamp(level, kTrace, kFatal))
#ifdef XIAOLOG_SPDLOG_SUPPORT
      ,
      func_(func)
#endif
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7) << "[" << func << "] ";
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
}
Logger::Logger(SourceFile file, int line, bool)
    : sourceFile_(file), fileLine_(line), level_(kFatal)
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
    if (errno != 0) // errno 是一个全局变量，用于存储最近一次系统调用的错误代码
    {
        logStream_ << strerror_tl(errno) << " (errno=" << errno << ") ";
    }
}

Logger::Logger() : level_(kInfo)
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
}
Logger::Logger(LogLevel level) : level_(std::clamp(level, kTrace, kFatal))
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
}
Logger::Logger(bool) : level_(kFatal)
{
    formatTime();
    logStream_ << T(logLevelStr[level_], 7);
#ifdef XIAOLOG_SPDLOG_SUPPORT
    spdLogMessageOffset_ = logStream_.bufferLength();
#endif
    if (errno != 0)
    {
        logStream_ << strerror_tl(errno) << " (errno=" << errno << ") ";
    }
}

bool Logger::hasSpdLogSupport()
{
#ifdef XIAOLOG_SPDLOG_SUPPORT
    return true;
#else
    return false;
#endif
}

#ifdef XIAOLOG_SPDLOG_SUPPORT

#endif

std::shared_ptr<spdlog::logger> Logger::getDefaultSpdLogger(int index)
{
#ifdef XIAOLOG_SPDLOG_SUPPORT

#else
    (void)index;
    return {};
#endif
}

std::shared_ptr<spdlog::logger> Logger::getSpdLogger(int index)
{
#ifdef XIAOLOG_SPDLOG_SUPPORT

#else
    (void)index;
    return {};
#endif
}

RawLogger::~RawLogger()
{

#ifdef XIAOLOG_SPDLOG_SUPPORT

#endif

    if (index_ < 0)
    {
        auto &oFunc = Logger::outputFunc_();
        if (!oFunc)
            return;
        oFunc(logStream_.bufferData(), logStream_.bufferLength());
    }
    else
    {
        auto &oFunc = Logger::outputFunc_(index_);
        if (!oFunc)
            return;
        oFunc(logStream_.bufferData(), logStream_.bufferLength());
    }
}

Logger::~Logger()
{

#ifdef XIAOLOG_SPDLOG_SUPPORT

#endif
    if (sourceFile_.data_)
        logStream_ << T(" - ", 3) << sourceFile_ << ":" << fileLine_ << '\n';
    else
        logStream_ << '\n';
    if (index_ < 0)
    {
        auto &oFunc = Logger::outputFunc_();
        if (!oFunc)
            return;
        oFunc(logStream_.bufferData(), logStream_.bufferLength());
        if (level_ >= kError)
            Logger::flushFunc_()();
    }
    else
    {
        auto &oFunc = Logger::outputFunc_(index_);
        if (!oFunc)
            return;
        oFunc(logStream_.bufferData(), logStream_.bufferLength());
        if (level_ >= kError)
            Logger::flushFunc_(index_)();
    }
}
LogStream &Logger::stream()
{
    return logStream_;
}