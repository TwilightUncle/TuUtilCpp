#include <gtest/gtest.h>
#include <utility.hpp>

// 5桁の10進数の想定で考える(コンテナの一要素に入る最大値は9)
static constexpr char max_num = 10;

using test_class = tudb::carry_over_container<char, 5>;

static auto plus(char l, char r)
{
    return (l + r >= max_num)
        ? std::pair{std::array<char, 2>{l + r - max_num, (char)1}, true}
        : std::pair{std::array<char, 2>{l + r, (char)0}, false};
}

constexpr auto mul = [](char l, char r)
{
    const char res = l * r;
    return std::pair{
        std::array<char, 2>{res % max_num, res / max_num},
        res >= max_num
    };
};

static auto inst_to_string(const test_class& inst)
{
    char str[6] = {};
    for (int i = 0; i < 5; i++) str[4-i] = inst[i] + '0';
    return std::string(str);
}

// 10進数の足し算・引き算のひっ算を再現
TEST(tudbcpptest, CarryOverRunTest)
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

    // 12345 + 8 = 12353
    ASSERT_STREQ(case1.data(), "12353");
    // 89746 + 12353 = 102099(最大桁は想定サイズ外の値なので無視される)
    ASSERT_STREQ(case2.data(), "02099");
    // 2099 * 9 = 18891
    ASSERT_STREQ(case3.data(), "18891");
    // 18891 * 256 = 4836096(上位2桁は想定サイズ外の値なので無視される)
    ASSERT_STREQ(case4.data(), "36096");
    ASSERT_STREQ(case5.data(), "12345");
};
