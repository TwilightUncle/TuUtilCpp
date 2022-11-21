#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, BigIntMethodTest)
{
    auto case1 = tudb::big_int<2>::plus((std::numeric_limits<unsigned long long>::max)() - 1, 1);
    auto case2 = tudb::big_int<2>::plus((std::numeric_limits<unsigned long long>::max)(), 1);
    auto case3 = tudb::big_int<2>::plus((std::numeric_limits<unsigned long long>::max)(), 2);
    auto case4 = tudb::big_int<2>::mul((std::numeric_limits<unsigned long long>::max)() / 2, 2);
    auto case5 = tudb::big_int<2>::mul((std::numeric_limits<unsigned long long>::max)() / 2 + 1, 2);
    auto case6 = tudb::big_int<2>::mul((std::numeric_limits<unsigned long long>::max)(), 2);

    ASSERT_EQ(case1.first[0], (std::numeric_limits<unsigned long long>::max)());
    ASSERT_EQ(case1.first[1], 0);
    ASSERT_EQ(case1.second, false);
    ASSERT_EQ(case2.first[0], 0);
    ASSERT_EQ(case2.first[1], 1);
    ASSERT_EQ(case2.second, true);
    ASSERT_EQ(case3.first[0], 1);
    ASSERT_EQ(case3.first[1], 1);
    ASSERT_EQ(case3.second, true);
    ASSERT_EQ(case4.first[0], (std::numeric_limits<unsigned long long>::max)() - 1);
    ASSERT_EQ(case4.first[1], 0);
    ASSERT_EQ(case4.second, false);
    ASSERT_EQ(case5.first[0], 0);
    ASSERT_EQ(case5.first[1], 1);
    ASSERT_EQ(case5.second, true);
    ASSERT_EQ(case6.first[0], (std::numeric_limits<unsigned long long>::max)() - 1);
    ASSERT_EQ(case6.first[1], 1);
    ASSERT_EQ(case6.second, true);
}

TEST(tudbcpptest, BigIntOperatorTest)
{
    // 上位の桁が0の場合はサイズが異なっていても比較対象として考慮しないものとする(intやshortのサイズが異なる整数型の比較に準拠)
    constexpr auto case1 = tudb::big_int<5>{~(unsigned)0, (unsigned)1, (unsigned)0, (unsigned)1, (unsigned)0}
                        == tudb::big_int<4>{~(unsigned)0, (unsigned)1, (unsigned)0, (unsigned)1};
    constexpr auto case2 = tudb::big_int<4>{~(unsigned)1, (unsigned)1, (unsigned)0, (unsigned)1}
                        != tudb::big_int<4>{~(unsigned)0, (unsigned)1, (unsigned)0, (unsigned)1};
    constexpr auto case3 = tudb::big_int<4>{~(unsigned)1, (unsigned)1, (unsigned)0, (unsigned)1}
                        <  tudb::big_int<5>{~(unsigned)0, (unsigned)1, (unsigned)0, (unsigned)1, (unsigned)0};
    constexpr auto case4 = tudb::big_int<4>{~(unsigned)1, (unsigned)1, (unsigned)1, (unsigned)1}
                        >= tudb::big_int<5>{~(unsigned)0, (unsigned)1, (unsigned)0, (unsigned)1, (unsigned)0};

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}
