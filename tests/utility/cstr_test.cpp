#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, CstrConcatTest)
{
    constexpr auto case1 = tudb::concat(tudb::cstr{"abc"}, tudb::cstr{"defghi"});
    constexpr auto case2 = tudb::concat(tudb::cstr{"abc"}, tudb::cstr{"1234"}, tudb::cstr{"zyxwv"});

    ASSERT_STREQ(case1.buf, "abcdefghi");
    ASSERT_STREQ(case2.buf, "abc1234zyxwv");
}

TEST(tudbcpptest, CstrOperatorTest)
{
    constexpr auto case1 = tudb::cstr{"abc"} == tudb::cstr{"abc"};
    constexpr auto case2 = tudb::cstr{"abc"} == tudb::cstr{"adc"};
    constexpr auto case3 = tudb::cstr{"abc"} == tudb::cstr{"abcd"};

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
}

TEST(tudbcpptest, CstrGetDigitTest)
{
    constexpr auto case1 = tudb::get_digit<255>();
    constexpr auto case2 = tudb::get_digit<255, 2>();
    constexpr auto case3 = tudb::get_digit<255, 8>();
    constexpr auto case4 = tudb::get_digit<255, 16>();
    constexpr auto case5 = tudb::get_digit<-255>();
    constexpr auto case6 = tudb::get_digit<-255, 2>();
    constexpr auto case7 = tudb::get_digit<-255, 8>();
    constexpr auto case8 = tudb::get_digit<-255, 16>();

    ASSERT_EQ(case1, 3);
    ASSERT_EQ(case2, 8);
    ASSERT_EQ(case3, 3);
    ASSERT_EQ(case4, 2);
    ASSERT_EQ(case5, 3);
    ASSERT_EQ(case6, 8);
    ASSERT_EQ(case7, 3);
    ASSERT_EQ(case8, 2);
}

TEST(tudbcpptest, CstrIntegralToStringTest)
{
    constexpr auto case1 = tudb::to_string<(unsigned int)154352>();
    constexpr auto case2 = tudb::to_string<(long)-54362>();
    constexpr auto case3 = tudb::to_string<(unsigned char)0b100, 2>();
    constexpr auto case4 = tudb::to_string<(short)-0777, 8>();
    constexpr auto case5 = tudb::to_string<(unsigned long)0xffffffff, 16>(); 
    constexpr auto case6 = tudb::to_string<100, 10, true>(); 
    constexpr auto case7 = tudb::to_string<-0b1111, 2, true>(); 
    constexpr auto case8 = tudb::to_string<077777, 8, true>(); 
    constexpr auto case9 = tudb::to_string<-0xfffff, 16, true>();

    ASSERT_STREQ(case1.buf, "154352");
    ASSERT_STREQ(case2.buf, "-54362");
    ASSERT_STREQ(case3.buf, "100");
    ASSERT_STREQ(case4.buf, "-777");
    ASSERT_STREQ(case5.buf, "FFFFFFFF");
    ASSERT_STREQ(case6.buf, "100");
    ASSERT_STREQ(case7.buf, "-0b1111");
    ASSERT_STREQ(case8.buf, "077777");
    ASSERT_STREQ(case9.buf, "-0xFFFFF");
}
