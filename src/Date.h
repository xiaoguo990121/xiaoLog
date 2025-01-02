/**
 * @file Date.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-03
 *
 *
 */

#pragma once

#include <xiaoLog/exports.h>
#include <stdint.h>
#include <string>

#define MICRO_SECONDS_PRE_SEC 1000000LL

namespace xiaoLog
{
    /**
     * @brief This class represents a time point.
     *
     */
    class XIAOLOG_EXPORT Date
    {

    public:
        Date() : microSecondsSinceEpoch_(0) {};

        /**
         * @brief Construct a new Date object
         *
         * @param microSec The microseconds from 1970-01-01 00:00:00.
         */
        explicit Date(int64_t microSec) : microSecondsSinceEpoch_(microSec) {};

        /**
         * @brief Construct a new Date object
         *
         * @param year
         * @param month
         * @param day
         * @param hour
         * @param minute
         * @param second
         * @param microSecond
         */
        Date(unsigned int year,
             unsigned int month,
             unsigned int day,
             unsigned int hour = 0,
             unsigned int minute = 0,
             unsigned int second = 0,
             unsigned int microSecond = 0);

        /**
         * @brief Create a Date object that represents the current time.
         *
         * @return const Date
         */
        static const Date date();

        /**
         * @brief Same as the date() method.
         *
         * @return const Date
         */
        static const Date now()
        {
            return Date::date();
        }

        static int64_t timezoneOffset()
        {
            static int64_t offset = -(
                Date::fromDbStringLocal("1970-01-03 00:00:00").secondsSinceEpoch() -
                2LL * 3600LL * 24LL);
            return offset;
        }

        /**
         * @brief Return a new Date instance that represents the time after
         * some seconds from *this.
         *
         * @param second
         * @return const Date
         */
        const Date after(double second) const;

        /**
         * @brief Return a new Date instance that can equals to *this, but with zero
         * microseconds.
         *
         * @return const Date
         */
        const Date roundSecond() const;

        /**
         * @brief Return a new Date instance that equals to *this, but with zero
         * hours, minutes, seconds and microseconds.
         *
         * @return const Date
         */
        const Date roundDay() const;

        ~Date() {};

        /**
         * @brief Return true if the time point is equal to another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator==(const Date &date) const
        {
            return microSecondsSinceEpoch_ == date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Return true if the time point is not equal to another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator!=(const Date &date) const
        {
            return microSecondsSinceEpoch_ != date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Return true if the time point is earlier than another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator<(const Date &date) const
        {
            return microSecondsSinceEpoch_ < date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Return true if the time point is later than another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator>(const Date &date) const
        {
            return microSecondsSinceEpoch_ > date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Return true if the time point is not earlier than another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator>=(const Date &date) const
        {
            return microSecondsSinceEpoch_ >= date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Return true if the time point is not later than another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool operator<=(const Date &date) const
        {
            return microSecondsSinceEpoch_ <= date.microSecondsSinceEpoch_;
        }

        /**
         * @brief Get the number of milliseconds since 1970-01-01 00:00.
         *
         * @return int64_t
         */
        int64_t microSecondsSinceEpoch() const
        {
            return microSecondsSinceEpoch_;
        }

        /**
         * @brief Get the number of seconds since 1970-01-01 00:00.
         *
         * @return int64_t
         */
        int64_t secondsSinceEpoch() const
        {
            return microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC;
        }

        /**
         * @brief Get the tm struct for the time point.
         *
         * @return struct tm
         */
        struct tm tmStruct() const;

        /**
         * @brief Generate a UTC time string
         *
         * @param showMicroseconds whether the microseconds are returned.
         * @return std::string
         * @note Examples:
         *    - "20250102 01:01:25" if the @p showMicroseconds is false
         *    - "20250102 01:01:25:102414" if the @p showMicroseconds is true
         */
        std::string toFormattedString(bool showMicroseconds) const;

        /**
         * @brief Generate a UTC time string formatted by the @p fmtStr
         *
         * @param fmtStr
         * @param showMicroseconds
         * @return std::string
         * @note Examples:
         *     - "20250102 01:01:25" if the @p fmtStr is "%Y-%m-%d %H:%M:%S" and the
         *       @p showMicroseconds is false
         *     - "20250102 01:01:25:102414" if the @p fmtStr is "%Y-%m-%d %H:%M:%S"
         *      and the @p showMicroseconds is true
         */
        std::string toCustomFormattedString(const std::string &fmtStr,
                                            bool showMicroseconds = false) const;

        /**
         * @brief Generate a local time zone string, the format of the string is
         * sams as the method toFormattedString
         *
         * @param showMicroseconds
         * @return std::string
         */
        std::string toFormattedStringLocal(bool showMicroseconds) const;

        /**
         * @brief Generate a local time zone string formatted by the @p fmtStr
         *
         * @param fmtStr
         * @param showMicroseconds
         * @return std::string
         */
        std::string toCustomFormattedStringLocal(const std::string &fmtStr,
                                                 bool showMicroseconds = false) const;

        /**
         * @brief Generate a local time zone string for database.
         * @note Examples:
         *      - "2025-01-02" if hours, minutes, seconds and microseconds are zero
         *      - "2025-01-02 01:01:24" if the microsecond is zero
         *      - "2025-01-02 01:01:24:102414" if the microsecond is not zrro
         *
         * @return std::string
         */
        std::string toDbStringLocal() const;

        /**
         * @brief Generate a UTC time string for database.
         *
         * @return std::string
         */
        std::string toDbString() const;

        /**
         * @brief From DB string to xiaoLog local time zone.
         *
         * @param datetime
         * @return Date
         */
        static Date fromDbStringLocal(const std::string &datetime);

        /**
         * @brief From DB string to a xiaoLog UTC time.
         *
         * @param datetime
         * @return Date
         */
        static Date fromDbString(const std::string &datetime);

        /**
         * @brief Generate a UTC time string.
         *
         * @param fmtStr
         * @param str
         * @param len
         */
        void toCustomFormattedString(const std::string &fmtStr,
                                     char *str,
                                     size_t len) const;

        /**
         * @brief Return true if the time point is in a same second as another.
         *
         * @param date
         * @return true
         * @return false
         */
        bool isSameSecond(const Date &date) const
        {
            return microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC ==
                   date.microSecondsSinceEpoch_ / MICRO_SECONDS_PRE_SEC;
        }

        void swap(Date &that)
        {
            std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
        }

    private:
        int64_t microSecondsSinceEpoch_{0};
    };

} // namespace xiaoLog
