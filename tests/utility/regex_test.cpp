#include <gtest/gtest.h>
#include <utility.hpp>


using namespace std::string_view_literals;
TEST(tudbcpptest, RegexFunctionTest)
{
    using test_type = tudb::regex<"abcdef">;
    constexpr auto empty_inst = tudb::empty_regex{};
    // // どの文法を使用するか指定がなかった場合、ECMAScriptのビットが含まれているか確認
    // constexpr auto inst1 = tudb::regex{"abcde", std::regex_constants::syntax_option_type::icase};

    // EXPECT_EQ(inst1.syntax, std::regex_constants::syntax_option_type::ECMAScript | std::regex_constants::syntax_option_type::icase);
    constexpr auto case1 = tudb::is_collect_regex_back_slash("a\\0\\\\a\\\\");
    constexpr auto case2 = tudb::is_collect_regex_back_slash("a\\0\\\\a\\");
    constexpr auto case3 = tudb::is_collect_regex_back_slash("\\a\\0\\\\a\\\\");
    constexpr auto case4 = tudb::is_collect_regex_back_slash("a\\0\\\\\\a\\\\");
    constexpr auto case5 = tudb::is_allowed_string("aaaab", true, "ab", "");
    constexpr auto case6 = tudb::is_allowed_string("\\0\\n\\\\", true, "", "0n\\");
    constexpr auto case7 = tudb::is_allowed_string("a\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case8 = tudb::is_allowed_string("a\\0\\\\a\\\\", false, "bc", "ds");
    constexpr auto case9 = tudb::is_allowed_string("ab\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case10 = tudb::is_allowed_string("a\\d\\0\\\\a\\\\", false, "bc", "ds");
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
    ASSERT_TRUE(case5);
    ASSERT_TRUE(case6);
    ASSERT_TRUE(case7);
    ASSERT_TRUE(case8);
    ASSERT_FALSE(case9);
    ASSERT_FALSE(case10);
}

TEST(tudbcpptest, RegexExtractBrancketTest)
{
    // 正常
    constexpr auto case11 = tudb::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)]rs", 3>::value;
    constexpr auto case12 = tudb::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)}rs", 3>::value;
    constexpr auto case13 = tudb::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\))rs", 3>::value;
    constexpr auto case14 = tudb::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)>rs", 3>::value;
    EXPECT_STREQ(case11.data(), "defg{hijk}lm\\](opq)");
    EXPECT_STREQ(case12.data(), "defg\\{hijk\\}lm](opq)");
    EXPECT_STREQ(case13.data(), "defg{hijk\\}lm](opq\\)");
    EXPECT_STREQ(case14.data(), "defg{hijk\\}lm](opq\\)");

    // 異常(対応する閉じ括弧が存在しない) コメントアウトを外すとコンパイルエラーが発生
    // constexpr auto case15 = tudb::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)\\]rs", 3>::value;
    // constexpr auto case16 = tudb::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)\\}rs", 3>::value;
    // constexpr auto case17 = tudb::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\)\\)rs", 3>::value;
    // constexpr auto case18 = tudb::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)\\>rs", 3>::value;
}

TEST(tudbcpptest, RegexCharRangeParseTest)
{
    // 文字クラス,バックスラッシュ,文字範囲指定なし
    constexpr auto case19 = tudb::get_regex_char_range_matcher<"-abc.">()(".c-aba");
    constexpr auto case20 = tudb::get_regex_char_range_matcher<"abc.-">()(".c-aba");
    constexpr auto case21 = tudb::get_regex_char_range_matcher<"-abc.">()("b.c-d");
    constexpr auto case22 = tudb::get_regex_char_range_matcher<"-abc.">()("def");
    constexpr auto case23 = tudb::get_regex_char_range_matcher<"^-abc">()("cab");
    constexpr auto case24 = tudb::get_regex_char_range_matcher<"^-abc">()("bcd");
    constexpr auto case25 = tudb::get_regex_char_range_matcher<"^-abc">()("def^");
    EXPECT_TRUE(case19);
    EXPECT_TRUE(case20);
    EXPECT_FALSE(case21);
    EXPECT_FALSE(case22);
    EXPECT_FALSE(case23);
    EXPECT_FALSE(case24);
    EXPECT_TRUE(case25);

    // 文字クラス指定あり
    constexpr auto case26 = tudb::get_regex_char_range_matcher<"\\d">()("0123456789");
    constexpr auto case27 = tudb::get_regex_char_range_matcher<"\\d">()("0123456789a");
    constexpr auto case28 = tudb::get_regex_char_range_matcher<"^\\d">()("0aabc_%&\t\\[");
    constexpr auto case29 = tudb::get_regex_char_range_matcher<"^\\d">()("abc_%&\t\\[");
    constexpr auto case30 = tudb::get_regex_char_range_matcher<"\\D">()("0aabc_%&\t\\[");
    constexpr auto case31 = tudb::get_regex_char_range_matcher<"\\D">()("aabc_%&\t\\[");
    constexpr auto case32 = tudb::get_regex_char_range_matcher<"^\\D">()("0123456789");
    constexpr auto case33 = tudb::get_regex_char_range_matcher<"^\\D">()("0123456789a");

    // 生成した関数を保持できるかも確認(レンジのテスト)
    constexpr auto range_matcher1 = tudb::get_regex_char_range_matcher<"0-5a-dA\\-E\\t">();

    EXPECT_TRUE(case26);
    EXPECT_FALSE(case27);
    EXPECT_FALSE(case28);
    EXPECT_TRUE(case29);
    EXPECT_FALSE(case30);
    EXPECT_TRUE(case31);
    EXPECT_TRUE(case32);
    EXPECT_FALSE(case33);
    EXPECT_TRUE(range_matcher1("0123451bcdA-E\t"));
    EXPECT_FALSE(range_matcher1("0123451bcdA-Et"));
    EXPECT_FALSE(range_matcher1("0123451bcdABCDE"));
}

TEST(tudbcpptest, RegexQuantifierParseTest)
{
    using type1 = tudb::regex_quantifier_perser<"abcd*e", 4>;
    using type2 = tudb::regex_quantifier_perser<"abcd*", 4>;
    using type3 = tudb::regex_quantifier_perser<"abcd*?", 4>;
    using type4 = tudb::regex_quantifier_perser<"abcd+", 4>;
    using type5 = tudb::regex_quantifier_perser<"abcd?", 4>;
    using type6 = tudb::regex_quantifier_perser<"abcd{2}e", 4>;
    using type7 = tudb::regex_quantifier_perser<"abcd{2,}", 4>;
    using type8 = tudb::regex_quantifier_perser<"abcd{2,5}", 4>;
    using type9 = tudb::regex_quantifier_perser<"abcd{2}?e", 4>;
    using type10 = tudb::regex_quantifier_perser<"abcd{2,}?", 4>;
    using type11 = tudb::regex_quantifier_perser<"abcd{2,5}?", 4>;
    EXPECT_FALSE(type1::negative);
    EXPECT_FALSE(type2::negative);
    EXPECT_TRUE(type3::negative);
    EXPECT_FALSE(type4::negative);
    EXPECT_FALSE(type5::negative);
    EXPECT_FALSE(type6::negative);
    EXPECT_FALSE(type7::negative);
    EXPECT_FALSE(type8::negative);
    EXPECT_TRUE(type9::negative);
    EXPECT_TRUE(type10::negative);
    EXPECT_TRUE(type11::negative);
    EXPECT_EQ(type1::min_count, 0);
    EXPECT_EQ(type1::max_count, std::string_view::npos);
    EXPECT_EQ(type2::min_count, 0);
    EXPECT_EQ(type3::max_count, std::string_view::npos);
    EXPECT_EQ(type4::min_count, 1);
    EXPECT_EQ(type4::max_count, std::string_view::npos);
    EXPECT_EQ(type5::min_count, 0);
    EXPECT_EQ(type5::max_count, 1);
    EXPECT_EQ(type6::min_count, 2);
    EXPECT_EQ(type6::max_count, 2);
    EXPECT_EQ(type7::min_count, 2);
    EXPECT_EQ(type7::max_count, std::string_view::npos);
    EXPECT_EQ(type8::min_count, 2);
    EXPECT_EQ(type8::max_count, 5);
}
