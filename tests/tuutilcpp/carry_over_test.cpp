#include <gtest/gtest.h>
#include <tuutilcpp/utility.hpp>

// 5桁の10進数の想定で考える(コンテナの一要素に入る最大値は9)
static constexpr char max_num = 10;

using test_class = tudb::carry_over_container<char, 5>;
using test_class2 = tudb::carry_over_container<int, 5>;

static auto plus(char l, char r)
{
    return (l + r >= max_num)
        ? std::tuple{(char)(l + r - max_num), (char)1, true}
        : std::tuple{(char)(l + r), (char)0, false};
}

constexpr auto mul = [](char l, char r)
{
    const char res = l * r;
    return std::tuple{(char)(res % max_num), (char)(res / max_num), res >= max_num};
};

template <std::convertible_to<char> T>
static auto inst_to_string(const tudb::carry_over_container<T, 5>& inst)
{
    char str[6] = {};
    for (int i = 0; i < 5; i++) str[4-i] = (char)(inst[i] + '0');
    return std::string(str);
}

// 10進数の足し算・引き算のひっ算を再現
TEST(tudbcpptest, CarryOverWithTest)
{
    auto inst1 = test_class{(char)5, (char)4, (char)3, (char)2, (char)1};
    inst1 = inst1.with_carry_up(plus, (char)8);
    auto case1 = inst_to_string(inst1);

    auto inst2 = test_class{(char)6, (char)4, (char)7, (char)9, (char)8};
    inst1 = inst1.with_carry_up(plus, inst2);
    auto case2 = inst_to_string(inst1);

    inst1 = inst1.with_carry_up_all(mul, (char)9, plus);
    auto case3 = inst_to_string(inst1);

    auto inst3 = test_class{(char)6, (char)5, (char)2};
    inst1 = inst1.with_carry_up_all(mul, inst3, plus);
    auto case4 = inst_to_string(inst1);

    inst1.set(test_class{(char)5, (char)4, (char)3, (char)2, (char)1});
    auto case5 = inst_to_string(inst1);

    // バッファに格納されている値が、変換可能な型であれば、with_carry_upを呼び出せることの確認
    inst1.set_with_carry_up<test_class>(plus, test_class2{6, 4, 7, 9, 8});
    auto case6 = inst_to_string(inst1);

    // 12345 + 8 = 12353
    ASSERT_STREQ(case1.data(), "12353");
    // 89746 + 12353 = 102099(最大桁は想定サイズ外の値なので無視される)
    ASSERT_STREQ(case2.data(), "02099");
    // 2099 * 9 = 18891
    ASSERT_STREQ(case3.data(), "18891");
    // 18891 * 256 = 4836096(上位2桁は想定サイズ外の値なので無視される)
    ASSERT_STREQ(case4.data(), "36096");
    ASSERT_STREQ(case5.data(), "12345");
    // 12345 + 89746 = 012091
    ASSERT_STREQ(case6.data(), "02091");
};

TEST(tudbcpptest, CarryOverHelperTest)
{
    constexpr auto inst1 = tudb::carry_over_container<std::uint32_t, 5>{2u, 1ul << 20 | 1ul << 2, 1ul << 16, 0u, 0u};
    constexpr auto case1 = tudb::count_using_size(tudb::carry_over_container<std::uint16_t, 3>{0u, 0u, 0u});
    constexpr auto case2 = tudb::count_using_size(inst1);
    constexpr auto case3 = tudb::count_using_size(tudb::carry_over_container<std::uint32_t, 5>{0u, 1u, 1u, 0u, 1u});
    constexpr auto case4 = tudb::convert_diff_size_buffer<std::uint32_t>(tudb::convert_diff_size_buffer<std::uint16_t>(inst1));
    // 16 * 10 bitと、左記を内包可能な64 * 3ビットは異なっているため、最終的なバッファの最大ビット数は増える
    constexpr auto case5 = tudb::convert_diff_size_buffer<std::uint32_t>(tudb::convert_diff_size_buffer<std::uint64_t>(tudb::convert_diff_size_buffer<std::uint16_t>(inst1)));
    constexpr auto inst2 = tudb::carry_over_container<std::uint32_t, 6>{2u, 1ul << 20 | 1ul << 2, 1ul << 16, 0u, 0u, 0u};

    ASSERT_EQ(case1, 1);
    ASSERT_EQ(case2, 3);
    ASSERT_EQ(case3, 5);
    ASSERT_EQ(inst1, case4);
    ASSERT_EQ(case5, inst2);
}
