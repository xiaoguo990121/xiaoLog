#include <xiaoLog/Date.h>
#include <gtest/gtest.h>

using namespace xiaoLog;
TEST(Date, constructorTest)
{
    EXPECT_STREQ("1985-01-01 00:00:00",
                 Date(1985, 1, 1)
                     .toCustomFormattedStringLocal("%Y-%m-%d %H:%M:%S")
                     .c_str());
    EXPECT_STREQ("2004-02-29 00:00:00.000000",
                 Date(2004, 2, 29)
                     .toCustomFormattedStringLocal("%Y-%m-%d %H:%M:%S", true)
                     .c_str());
    EXPECT_STRNE("2001-02-29 00:00:00.000000",
                 Date(2001, 2, 29)
                     .toCustomFormattedStringLocal("%Y-%m-%d %H:%M:%S", true)
                     .c_str());
    EXPECT_STREQ("2018-01-01 00:00:00.000000",
                 Date(2018, 1, 1, 12, 12, 12, 2321)
                     .roundDay()
                     .toCustomFormattedStringLocal("%Y-%m-%d %H:%M:%S", true)
                     .c_str());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}