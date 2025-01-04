/**
 * @file AsyncFileLogger.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-04
 *
 *
 */

#pragma once

#include <xiaoLog/exports.h>
#include <src/NonCopyable.h>
#include <src/Date.h>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace xiaoLog
{
    using StringPtr = std::shared_ptr<std::string>;
    using StringPtrQueue = std::queue<StringPtr>;

    /**
     * @brief This class implements utility functions for writing logs
     * to files asynchronously.
     *
     */
    class XIAOLOG_EXPORT AsyncFileLogger : NonCopyable
    {
    public:
        /**
         * @brief Write the message to the log file.
         *
         * @param msg
         * @param len
         */
        void output(const char *msg, const uint64_t len);

        /**
         * @brief Flush data from memory buffer to the log file.
         *
         */
        void flush();

        /**
         * @brief Strat writing log files.
         *
         */
        void startLogging();

        /**
         * @brief Set the size limit of log files. When the log file size reaches
         * the limit, the log file is switched.
         *
         * @param limit
         */
        void setFileSizeLimit(uint64_t limit)
        {
            sizeLimit_ = limit;
        }

        /**
         * @brief Set the max number of log files. When the number exceed the
         * limit, the oldest log file will be deleted.
         *
         * @param maxFiles
         */
        void setMaxFiles(size_t maxFiles)
        {
            maxFiles_ = maxFiles;
        }

        /**
         * @brief Set whether to switch the log file when the AsyncFileLogger object
         * is destroyed. If this flag is set to true, the log file is not switched
         * when the AsyncFileLogger object is destroyed.
         *
         * @param flag
         */
        void setSwitchOnLimitOnly(bool flag = true)
        {
            switchOnLimitOnly_ = flag;
        }

        void setFileName(const std::string &baseName,
                         const std::string &extName = ".log",
                         const std::string &path = "./")
        {
            fileBaseName_ = baseName;
            extName[0] == '.' ? fileExtName_ = extName
                              : fileExtName_ = std::string(".") + extName;
            filePath_ = path;
            if (filePath_.length() == 0)
                filePath_ = "./";
            if (filePath_[filePath_.length() - 1] != '/')
                filePath_ = filePath_ + "/";
        }
        ~AsyncFileLogger();
        AsyncFileLogger();

    protected:
        std::mutex mutex_;
        std::condition_variable cond_;
        StringPtr logBufferPtr_;
        StringPtr nextBufferPtr_;
        StringPtrQueue writerBuffers_;
        StringPtrQueue tmpBuffers_;
        void writeLogToFile(const StringPtr buf);
        std::unique_ptr<std::thread> threadPtr_;
        bool stopFlag_{false};
        void logThreadFunc();
        std::string filePath_{"./"};
        std::string fileBaseName_{"xiaoLog"};
        std::string fileExtName_{".log"};
        uint64_t sizeLimit_{20 * 1024 * 1024};
        bool switchOnLimitOnly_{false};
        size_t maxFiles_{0};

        class LoggerFile : NonCopyable
        {
        public:
            LoggerFile(const std::string &filePath,
                       const std::string &fileBaseName,
                       const std::string &fileExtName,
                       bool switchOnLimitOnly = false,
                       size_t maxFiles = 0);
            ~LoggerFile();
            void writeLog(const StringPtr buf);
            void open();
            void switchLog(bool openNewOne);
            uint64_t getLength();
            explicit operator bool() const
            {
                return fp_ != nullptr;
            }
            void flush();

        protected:
            void initFilenameQueue();
            void deleteOldFiles();

            FILE *fp_{nullptr};
            Date creationDate_;
            std::string fileFullName_;
            std::string filePath_;
            std::string fileBaseName_;
            std::string fileExtName_;
            static uint64_t fileSeq_;
            bool switchOnLimitOnly_{false};

            size_t maxFiles_{0};
            std::deque<std::string> filenameQueue_;
        };
        std::unique_ptr<LoggerFile> loggerFilePtr_;

        uint64_t lostCounter_{0};
        void swapBuffer();
    };
}