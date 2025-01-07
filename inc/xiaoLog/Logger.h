/**
 * @file Logger.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-04
 *
 *
 */

#pragma once

#include <xiaoLog/NonCopyable.h>
#include <xiaoLog/Date.h>
#include <xiaoLog/LogStream.h>
#include <xiaoLog/exports.h>
#include <functional>
#include <vector>

namespace spdlog
{
    class logger;
} // namespace spdlog

#include <memory>

#define XIAOLOG_IF_(cond) for (int _r = 0; _r == 0 && (cond); _r = 1)

namespace xiaoLog
{
    /**
     * @brief This class implements log functions.
     *
     */
    class XIAOLOG_EXPORT Logger : public NonCopyable
    {
    public:
        enum LogLevel
        {
            kTrace = 0,
            kDebug,
            kInfo,
            kWarn,
            kError,
            kFatal,
            kNumberOfLogLevels
        };

        /**
         * @brief Calculate of basename of source files in compile time.
         *
         */
        class SourceFile
        {
        public:
            template <int N>
            inline SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1)
            {
// std::cout << data_ << std::endl;
#ifndef _MSC_VER
                const char *slash = strrchr(data_, '/'); // 查找字符串最后一次出现的位置
#else
                const char *slash = strrchr(data_, '\\');
#endif
                if (slash)
                {
                    data_ = slash + 1;
                    size_ -= static_cast<int>(data_ - arr);
                }
            }

            explicit SourceFile(const char *filename = nullptr) : data_(filename)
            {
                if (!filename)
                {
                    size_ = 0;
                    return;
                }
#ifndef _MSC_VER
                const char *slash = strrchr(filename, '/');
#else
                const char *slash = strrchr(filename, '\\');
#endif
                if (slash)
                {
                    data_ = slash + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

            const char *data_;
            int size_;
        };
        Logger(SourceFile file, int line);
        Logger(SourceFile file, int line, LogLevel level);
        Logger(SourceFile file, int line, bool isSysErr);
        Logger(SourceFile file, int line, LogLevel level, const char *func);

        Logger();
        Logger(LogLevel level);
        Logger(bool isSysErr);

        ~Logger();
        Logger &setIndex(int index)
        {
            index_ = index;
            return *this;
        }
        LogStream &stream();

        /**
         * @brief Set the Output Function object
         *
         * @param outputFunc The function to output a log message.
         * @param flushFunc The function to flush.
         * @param index The channel index.
         * @note Logs are output to the standard output by default.
         */
        static void setOutputFunction(
            std::function<void(const char *msg, const uint64_t len)> outputFunc,
            std::function<void()> flushFunc,
            int index = -1)
        {
            if (index < 0)
            {
                outputFunc_() = outputFunc;
                flushFunc_() = flushFunc;
            }
            else
            {
                outputFunc_(index) = outputFunc;
                flushFunc_(index) = flushFunc;
            }
        }

        /**
         * @brief Set the Log Level object
         *
         * @param level
         */
        static void setLogLevel(LogLevel level)
        {
            logLevel_() = level;
        }

        /**
         * @brief Get the current log level.
         *
         * @return LogLevel
         */
        static LogLevel logLevel()
        {
            return logLevel_();
        }

        /**
         * @brief Check whether it shows local time or UTC time.
         *
         * @return true
         * @return false
         */
        static bool displayLocalTime()
        {
            return displayLocalTime_();
        }

        /**
         * @brief Set whether it shows local time or UTC time. the default is UTC.
         *
         * @param showLocalTime
         */
        static void setDisplayLocalTime(bool showLocalTime)
        {
            displayLocalTime_() = showLocalTime;
        }

        /**
         * @brief Check whether xiaoLog was build with spdlog support
         *
         * @return true
         * @return false
         */
        static bool hasSpdLogSupport();

        /**
         * @brief Enable logging with spdlog for the specified channel.
         *
         * @param index
         * @param logger
         */
        static void enableSpdLog(int index,
                                 std::shared_ptr<spdlog::logger> logger = {});

        inline static void enableSpdLog(std::shared_ptr<spdlog::logger> logger = {})
        {
            enableSpdLog(-1, logger);
        }

        /**
         * @brief Disable logging with spdlog for the specified channel.
         *
         * @param index
         */
        static void disableSpdLog(int index);

        static void disableSpdLog()
        {
            disableSpdLog(-1);
        }

        /**
         * @brief Get the Spd Logger object
         *
         * @param index
         * @return std::shared_ptr<spdlog::logger>
         */
        static std::shared_ptr<spdlog::logger> getSpdLogger(int index = -1);

        static std::shared_ptr<spdlog::logger> getDefaultSpdLogger(int index);

    protected:
        static void defaultOutputFunction(const char *msg, const uint64_t len)
        {
            fwrite(msg, 1, static_cast<size_t>(len), stdout);
        }
        static void defaultFlushFunction()
        {
            fflush(stdout);
        }
        void formatTime();
        static bool &displayLocalTime_()
        {
            static bool showLocalTime = false;
            return showLocalTime;
        }

        static LogLevel &logLevel_()
        {
#ifdef RELEASE
            static LogLevel loglevel = LogLevel::kInfo;
#else
            static LogLevel logLevel = LogLevel::kDebug;
#endif
            return logLevel;
        }
        static std::function<void(const char *msg, const uint64_t len)> &outputFunc_()
        {
            static std::function<void(const char *msg, const uint64_t len)>
                outputFunc = Logger::defaultOutputFunction;
            return outputFunc;
        }
        static std::function<void()> &flushFunc_()
        {
            static std::function<void()> flushFunc = Logger::defaultFlushFunction;
            return flushFunc;
        }
        static std::function<void(const char *msg, const uint64_t len)> &outputFunc_(size_t index)
        {
            static std::vector<
                std::function<void(const char *msg, const uint64_t len)>>
                outputFuncs;
            if (index < outputFuncs.size())
            {
                return outputFuncs[index];
            }
            while (index >= outputFuncs.size())
            {
                outputFuncs.emplace_back(outputFunc_());
            }
            return outputFuncs[index];
        }
        static std::function<void()> &flushFunc_(size_t index)
        {
            static std::vector<std::function<void()>> flushFuncs;
            if (index < flushFuncs.size())
            {
                return flushFuncs[index];
            }
            while (index >= flushFuncs.size())
            {
                flushFuncs.emplace_back(flushFunc_());
            }
            return flushFuncs[index];
        }

        friend class RawLogger;
        LogStream logStream_;
        Date date_{Date::now()};
        SourceFile sourceFile_;
        int fileLine_;
        LogLevel level_;
        int index_{-1};
        const char *func_{nullptr};
        std::size_t spdLogMessageOffset_{0};
    };
    class XIAOLOG_EXPORT RawLogger : public NonCopyable
    {
    public:
        ~RawLogger();
        RawLogger &setIndex(int index)
        {
            index_ = index;
            return *this;
        }
        LogStream &stream()
        {
            return logStream_;
        }

    private:
        LogStream logStream_;
        int index_{-1};
    };

#ifdef NDEBUG
#define LOG_TRACE                                                          \
    XIAOLOG_IF_(0)                                                         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .stream()
#else
#define LOG_TRACE                                                          \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kTrace)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .stream()
#define LOG_TRACE_TO(index)                                                \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kTrace)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .setIndex(index)                                                   \
        .stream()

#endif

#define LOG_DEBUG                                                          \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__) \
        .stream()
#define LOG_DEBUG_TO(index)                                                \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__) \
        .setIndex(index)                                                   \
        .stream()
#define LOG_INFO                                                       \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define LOG_INFO_TO(index)                                             \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) \
    xiaoLog::Logger(__FILE__, __LINE__)                                \
        .setIndex(index)                                               \
        .stream()
#define LOG_WARN                                                \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn) \
        .stream()
#define LOG_WARN_TO(index)                                      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn) \
        .setIndex(index)                                        \
        .stream()
#define LOG_ERROR                                                \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError) \
        .stream()
#define LOG_ERROR_TO(index)                                      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError) \
        .setIndex(index)                                         \
        .stream()
#define LOG_FATAL                                                \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal) \
        .stream()
#define LOG_FATAL_TO(index)                                      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal) \
        .setIndex(index)                                         \
        .stream()
#define LOG_SYSERR                            \
    xiaoLog::Logger(__FILE__, __LINE__, true) \
        .stream()
#define LOG_SYSERR_TO(index)                  \
    xiaoLog::Logger(__FILE__, __LINE__, true) \
        .setIndex(index)                      \
        .stream()

#define LOG_COMPACT_DEBUG                                               \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug) \
    xiaoLog::Logger(xiaoLog::Logger::kDebug).stream()
#define LOG_COMPACT_DEBUG_TO(index)                                     \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug) \
    xiaoLog::Logger(xiaoLog::Logger::kDebug).setIndex(index).stream()
#define LOG_COMPACT_INFO                                               \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) \
    xiaoLog::Logger().stream()
#define LOG_COMPACT_INFO_TO(index)                                     \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) \
    xiaoLog::Logger().setIndex(index).stream()
#define LOG_COMPACT_WARN \
    xiaoLog::Logger(xiaoLog::Logger::kWarn).stream()
#define LOG_COMPACT_WARN_TO(index) \
    xiaoLog::Logger(xiaoLog::Logger::kWarn).setIndex(index).stream()
#define LOG_COMPACT_ERROR \
    xiaoLog::Logger(xiaoLog::Logger::kError).stream()
#define LOG_COMPACT_ERROR_TO(index) \
    xiaoLog::Logger(xiaoLog::Logger::kError).setIndex(index).stream()
#define LOG_COMPACT_FATAL \
    xiaoLog::Logger(xiaoLog::Logger::kFatal).stream()
#define LOG_COMPACT_FATAL_TO(index) \
    xiaoLog::Logger(xiaoLog::Logger::kFatal).setIndex(index).stream()
#define LOG_COMPACT_SYSERR \
    xiaoLog::Logger(true).stream()
#define LOG_COMPACT_SYSERR_TO(index) \
    xiaoLog::Logger(true).setIndex(index).stream()

#define LOG_RAW xiaoLog::RawLogger().stream()
#define LOG_RAW_TO(index) xiaoLog::RawLogger().setIndex(index).stream()

#define LOG_TRACE_IF(cond)                                                  \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kTrace) && \
                (cond))                                                     \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__)  \
        .stream()
#define LOG_DEBUG_IF(cond)                                                  \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug) && \
                (cond))                                                     \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__)  \
        .stream()
#define LOG_INFO_IF(cond)                                                  \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) && \
                (cond))                                                    \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN_IF(cond) \
    XIAOLOG_IF_(cond)     \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn).stream()
#define LOG_ERROR_IF(cond) \
    XIAOLOG_IF_(cond)      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError).stream()
#define LOG_FATAL_IF(cond) \
    XIAOLOG_IF_(cond)      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal).stream()

#ifdef NDEBUG
#define DLOG_TRACE                                                         \
    XIAOLOG_IF_(0)                                                         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .stream()
#define DLOG_DEBUG                                                         \
    XIAOLOG_IF_(0)                                                         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__) \
        .stream()
#define DLOG_INFO  \
    XIAOLOG_IF_(0) \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN  \
    XIAOLOG_IF_(0) \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn).stream()
#define DLOG_ERROR \
    XIAOLOG_IF_(0) \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError).stream()
#define DLOG_FATAL \
    XIAOLOG_IF_(0) \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal).stream()

#define DLOG_TRACE_IF(cond)                                                \
    XIAOLOG_IF_(0)                                                         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .stream()
#define DLOG_DEBUG_IF(cond)                                                \
    XIAOLOG_IF_(0)                                                         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__) \
        .stream()
#define DLOG_INFO_IF(cond) \
    XIAOLOG_IF_(0)         \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN_IF(cond) \
    XIAOLOG_IF_(0)         \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn).stream()
#define DLOG_ERROR_IF(cond) \
    XIAOLOG_IF_(0)          \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError).stream()
#define DLOG_FATAL_IF(cond) \
    XIAOLOG_IF_(0)          \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal).stream()
#else
#define DLOG_TRACE                                                         \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kTrace)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__) \
        .stream()
#define DLOG_DEBUG                                                         \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug)    \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__) \
        .stream()
#define DLOG_INFO                                                      \
    XIAOLOG_IF_(xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn).stream()
#define DLOG_ERROR \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError).stream()
#define DLOG_FATAL \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal).stream()

#define DLOG_TRACE_IF(cond)                                                 \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kTrace) && \
                (cond))                                                     \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kTrace, __func__)  \
        .stream()
#define DLOG_DEBUG_IF(cond)                                                 \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kDebug) && \
                (cond))                                                     \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kDebug, __func__)  \
        .stream()
#define DLOG_INFO_IF(cond)                                                 \
    XIAOLOG_IF_((xiaoLog::Logger::logLevel() <= xiaoLog::Logger::kInfo) && \
                (cond))                                                    \
    xiaoLog::Logger(__FILE__, __LINE__).stream()
#define DLOG_WARN_IF(cond) \
    XIAOLOG_IF_(cond)      \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kWarn).stream()
#define DLOG_ERROR_IF(cond) \
    XIAOLOG_IF_(cond)       \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kError).stream()
#define DLOG_FATAL_IF(cond) \
    XIAOLOG_IF_(cond)       \
    xiaoLog::Logger(__FILE__, __LINE__, xiaoLog::Logger::kFatal).stream()
#endif
}