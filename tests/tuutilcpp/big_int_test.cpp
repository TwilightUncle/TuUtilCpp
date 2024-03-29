#include <gtest/gtest.h>
#include <tuutilcpp/utility.hpp>

using namespace tuutil::utility;

TEST(tudbcpptest, BigIntMethodTest)
{
    auto case1 = big_int<2>::plus((std::numeric_limits<unsigned long long>::max)() - 1, 1);
    auto case2 = big_int<2>::plus((std::numeric_limits<unsigned long long>::max)(), 1);
    auto case3 = big_int<2>::plus((std::numeric_limits<unsigned long long>::max)(), 2);
    auto case4 = big_int<2>::mul((std::numeric_limits<unsigned long long>::max)() / 2, 2);
    auto case5 = big_int<2>::mul((std::numeric_limits<unsigned long long>::max)() / 2 + 1, 2);
    auto case6 = big_int<2>::mul((std::numeric_limits<unsigned long long>::max)(), 2);
    auto case7 = big_int<2>::lshift(0x8000'0000'8000'0000u, 5u);
    auto case8 = big_int<2>::minus(4u, 5u);
    auto case9 = big_int<2>::minus(4u, 3u);
    constexpr auto case10 = big_int<2>{~0ull, 0b11u}.get_bit_digits();
    constexpr auto case11 = big_int<2>{0u, 0b010u}.get_bit_digits();
    constexpr auto case12 = big_int<2>{0b100u, 0u}.get_bit_digits();
    constexpr auto case13 = big_int<2>{0u, 0u}.get_bit_digits();
    constexpr auto case14 = big_int<2>{1u, 0u}.get_bit_digits();
    constexpr auto case15 = big_int<3>{1u, 0u, 0b100u}.get_bit_digits();

    ASSERT_EQ(std::get<0>(case1), (std::numeric_limits<unsigned long long>::max)());
    ASSERT_EQ(std::get<1>(case1), 0);
    ASSERT_EQ(std::get<2>(case1), false);
    ASSERT_EQ(std::get<0>(case2), 0);
    ASSERT_EQ(std::get<1>(case2), 1);
    ASSERT_EQ(std::get<2>(case2), true);
    ASSERT_EQ(std::get<0>(case3), 1);
    ASSERT_EQ(std::get<1>(case3), 1);
    ASSERT_EQ(std::get<2>(case3), true);
    ASSERT_EQ(std::get<0>(case4), (std::numeric_limits<unsigned long long>::max)() - 1);
    ASSERT_EQ(std::get<1>(case4), 0);
    ASSERT_EQ(std::get<2>(case4), false);
    ASSERT_EQ(std::get<0>(case5), 0);
    ASSERT_EQ(std::get<1>(case5), 1);
    ASSERT_EQ(std::get<2>(case5), true);
    ASSERT_EQ(std::get<0>(case6), (std::numeric_limits<unsigned long long>::max)() - 1);
    ASSERT_EQ(std::get<1>(case6), 1);
    ASSERT_EQ(std::get<2>(case6), true);
    ASSERT_EQ(std::get<0>(case7), (0x8000'0000ull << 5));
    ASSERT_EQ(std::get<1>(case7), 1ull << 4);
    ASSERT_EQ(std::get<2>(case7), true);
    ASSERT_EQ(std::get<0>(case8), ~0ull);
    ASSERT_EQ(std::get<1>(case8), 1);
    ASSERT_EQ(std::get<2>(case8), true);
    ASSERT_EQ(std::get<0>(case9), 1);
    ASSERT_EQ(std::get<1>(case9), 0);
    ASSERT_EQ(std::get<2>(case9), false);
    ASSERT_EQ(case10, 66);
    ASSERT_EQ(case11, 66);
    ASSERT_EQ(case12, 3);
    ASSERT_EQ(case13, 1);
    ASSERT_EQ(case14, 1);
    ASSERT_EQ(case15, 131);
}

TEST(tudbcpptest, BigIntOperatorTest)
{
    // 比較演算子のテスト
    // 上位の桁が0の場合はサイズが異なっていても比較対象として考慮しないものとする(intやshortのサイズが異なる整数型の比較に準拠)
    constexpr auto case1 = big_int<5>{~0ull, 1u, 0ull, 1ul, 0ull}
                        == big_int<4>{~0ull, 1ull, 0ull, 1ull};
    constexpr auto case2 = big_int<4>{~1ull, 1ull, 0ull, 1ull}
                        != big_int<4>{~0ull, 1ul, 0ull, 1ull};
    constexpr auto case3 = big_int<4>{~1ull, 1ull, 0ull, 1ull}
                        <  big_int<5>{~0ull, 1ull, 0ull, 1ull, 0ull};
    constexpr auto case4 = big_int<4>{~1ull, 1ull, 1ull, 1u}
                        >= big_int<5>{~0ull, 1ull, 0ull, 1ull, 0ull};
    constexpr auto case5 = !big_int<2>{};
    constexpr auto case6 = !big_int<2>{0u, 1u};
    constexpr auto case7 = big_int<2>{1u, 0u} <= 1;
    constexpr auto case8 = 5u > big_int<2>{4u, 0u};

    // 算術等(複合代入は二項演算子の中で利用しているので、そっち動けば問題なし)
    auto value1 = big_int<2>{};
    auto value2 = value1++;
    auto value3 = ++value1;
    // 桁上がりかつ定数式での使用確認
    constexpr auto value4 = ++big_int<2>{~0ull, 0ull};
    constexpr auto value5 = big_int<2>{0ull, 1ull};
    constexpr auto value6 = big_int<3>{~0ull, 0ull, 0ull} + big_int<2>{1ull, 1ull};
    constexpr auto value7 = big_int<2>{0ull, 2ull};
    constexpr auto value8 = big_int<2>{0x8000'0000'8000'0000u, 0u};
    constexpr auto value9 = value8 * big_int<3>{0x8000'0000'8000'0000u, 0u, 0u};
    constexpr auto value10 = big_int<2>{0x4000'0000'0000'0000u, 0x4000'0000'8000'0000u};
    constexpr auto value11 = big_int<4>{0x8000'0000'8000'0000u, 1u, 0u, 0u} << 135u;
    constexpr auto value12 = big_int<4>{0u, 0u, 0x8000'0000ull << 7, 0b11ull << 6};
    constexpr auto value13 = big_int<4>{0u, 3u, 0u, 0u} - big_int<3>{1u, 4u, 0u};
    constexpr auto value14 = big_int<4>{~0ull, ~0ull - 1, ~0ull, ~0ull};
    constexpr auto value15 = big_int<4>{0u, 5u, 0u, 0u} - big_int<3>{1u, 4u, 0u};
    constexpr auto value16 = big_int<4>{~0ull, 0u, 0u, 0u};
    constexpr auto value17 = big_int<2>{1ull, 1ull} + big_int<3>{~0ull, 0ull, 0ull};
    // 右シフトは内部的に左シフトを使用しているので、最終的なビットの桁の位置が等しいかだけ確認できればOK
    constexpr auto value18 = big_int<4>{0u, 1u, 0u, 1u} >> 192u;
    constexpr auto value19 = big_int<2>{1u, 0u};
    constexpr auto value20 = big_int<4>{0u, 1u, 0u, 1u} >> 126u;
    constexpr auto value21 = big_int<2>{0u, 0b100u};
    constexpr auto value22 = value19 / value21;
    constexpr auto value23 = value19 % value21;
    constexpr auto value24 = big_int<3>{~0ull, ~0ull, ~0ull} / big_int<2>{0u, 0b100u};
    constexpr auto value25 = big_int<3>{~0ull, ~0ull >> 2, 0u};
    constexpr auto value26 = big_int<3>{~0ull, ~0ull, ~0ull} % big_int<2>{0u, 0b100u};
    constexpr auto value27 = big_int<3>{~0ull, 0b11u, 0u};
    constexpr auto value28 = big_int<3>{~0ull, ~0ull, ~0ull} / big_int<3>{~0ull, ~0ull, ~0ull};
    constexpr auto value29 = big_int<3>{1u, 0u, 0u};

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
    ASSERT_TRUE(case5);
    ASSERT_FALSE(case6);
    ASSERT_TRUE(case7);
    ASSERT_TRUE(case8);
    ASSERT_EQ(big_int<2>{}, value2);
    ASSERT_EQ(value1, value3);
    ASSERT_EQ(value4, value5);
    ASSERT_EQ(value6, value7);
    ASSERT_EQ(value9, value10);
    ASSERT_EQ(value11, value12);
    ASSERT_EQ(value13, value14);
    ASSERT_EQ(value15, value16);
    ASSERT_EQ(value6, value17);
    ASSERT_EQ(value18, value19);
    ASSERT_EQ(value20, value21);
    ASSERT_EQ(value22, big_int<2>{});
    ASSERT_EQ(value23, value19);
    ASSERT_EQ(value24, value25);
    ASSERT_EQ(value26, value27);
    ASSERT_EQ(value28, value29);
}
