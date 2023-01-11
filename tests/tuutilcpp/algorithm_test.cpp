#include <gtest/gtest.h>
#include <tuutilcpp/utility.hpp>

TEST(tudbcpptest, GetDigitTest)
{
    constexpr auto case1 = tudb::get_digit<255>();
    constexpr auto case2 = tudb::get_digit<255, 2>();
    constexpr auto case3 = tudb::get_digit<255, 8>();
    constexpr auto case4 = tudb::get_digit<255, 16>();
    constexpr auto case5 = tudb::get_digit<-255>();
    constexpr auto case6 = tudb::get_digit<-255, 2>();
    constexpr auto case7 = tudb::get_digit<-255, 8>();
    constexpr auto case8 = tudb::get_digit<-255, 16>();
    constexpr auto case9 = tudb::get_digit<tudb::big_int<2>{~0ull, 80u}>();

    ASSERT_EQ(case1, 3);
    ASSERT_EQ(case2, 8);
    ASSERT_EQ(case3, 3);
    ASSERT_EQ(case4, 2);
    ASSERT_EQ(case5, 3);
    ASSERT_EQ(case6, 8);
    ASSERT_EQ(case7, 3);
    ASSERT_EQ(case8, 2);
    ASSERT_EQ(case9, 22);
}
