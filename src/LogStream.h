/**
 * @file LogStream.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-12-31
 *
 *
 */
#pragma once

#include "NonCopyable.h"
#include <xiaoLog/exports.h>

#include <string>

namespace xiaoLog
{
    namespace detail
    {
        static constexpr size_t kSmallBuffer{4000};
        static constexpr size_t kLargeBuffer{4000 * 1000};

        template <int SIZE>
        class XIAOLOG_EXPORT FixedBuffer : NonCopyable
        {
        public:
            FixedBuffer() : cur_(data_)
            {
                setCookie(cookieStart);
            }

            ~FixedBuffer()
            {
                setCookie(cookieEnd);
            }

            bool append(const char *buf, size_t len)
            {
                if ((size_t)(avail()) > len)
                {
                    memcpy(cur_, buf, len);
                    cur_ += len;
                    return true;
                }
                return false;
            }

            const char *data() const
            {
                return data_;
            }
            int length() const
            {
                return static_cast<int>(cur_ - data_);
            }

            // write to data_ directly
            char *current()
            {
                return cur_;
            }
            int avail() const
            {
                return static_cast<int>(end() - cur_);
            }
            void add(size_t len)
            {
                cur_ += len;
            }

            void reset()
            {
                cur_ = data_;
            }
            void zeroBuffer()
            {
                memset(data_, 0, sizeof(data_));
            }

            // for used by GDB
            const char *debugString();
            void setCookie(void (*cookie)())
            {
                cookie_ = cookie;
            }
            // for used by unit test
            std::string toString() const
            {
                return std::string(data_, length());
            }

        private:
            const char *end() const
            {
                return data_ + sizeof data_;
            }
            static void cookieStart();
            static void cookieEnd();

            void (*cookie_)();
            char data_[SIZE];
            char *cur_;
        };
    }
}