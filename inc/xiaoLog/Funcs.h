/**
 * @file Funcs.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-03
 *
 *
 */

#pragma once
#include <cstdint>
#include <algorithm>
#include <vector>

namespace xiaoLog
{
    // 用于网络字节序和主机字节序转换
    inline uint64_t hton64(uint64_t n)
    {
        static const int one = 1; // 0x00000001  如果是大端 00 00 00 01
        static const char sig = *(char *)&one;
        if (sig == 0)
            return n; // 大端直接返回
        char *ptr = reinterpret_cast<char *>(&n);
        std::reverse(ptr, ptr + sizeof(uint64_t));
        return n;
    }

    inline uint64_t ntoh64(uint64_t n)
    {
        return hton64(n);
    }

    inline std::vector<std::string> splitString(const std::string &s,
                                                const std::string &delimiter,
                                                bool acceptEmptyString = false)
    {
        if (delimiter.empty())
            return std::vector<std::string>{};
        std::vector<std::string> v;
        size_t last = 0;
        size_t next = 0;
        while ((next = s.find(delimiter, last)) != std::string::npos)
        {
            if (next > last || acceptEmptyString)
                v.push_back(s.substr(last, next - last));
            last = next + delimiter.length();
        }
        if (s.length() > last || acceptEmptyString)
            v.push_back(s.substr(last));
        return v;
    }
}