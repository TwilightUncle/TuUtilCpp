#include <gtest/gtest.h>
#include <tustrcpp/regex.hpp>

using namespace std::string_view_literals;

/**
 * @fn
 * @brief 許可リストまたは拒否リストとして指定したchar_listを参照し、targetが有効か検証する。バックスラッシュを含む場合、二つで一文字と認識する
 * @param target 検証する文字列
 * @param allow_or_deny allowの場合trueを指定、denyの場合falseを指定
 * @param char_list 許可または拒否文字のリスト。allow_or_denyによって拒否リストか許可リストか制御する
*/
static constexpr bool is_allowed_string(
    const std::string& target,
    bool allow_or_deny,
    const std::string& char_list,
    const std::string& bk_char_list,
    bool is_bk_escape = true
) {
    const auto collate_list = [&allow_or_deny](const std::string& list, char ch)->bool {
        // 許可リストの中から未発見のものがあった場合または、
        // 拒否リストの中から発見されたものがあった場合、失敗
        return (list.find_first_of(ch) == std::string::npos) == allow_or_deny;
    };

    for (int i = 0; i < target.size(); i++) {
        if ((target[i] != '\\' || !is_bk_escape) && collate_list(char_list, target[i])) return false;
        else if (target[i] == '\\' && i < target.size() - 1 && collate_list(bk_char_list, target[++i])) return false;
    }
    return true;
}

TEST(tustrcpptest, RegexFunctionTest)
{
    constexpr auto case1 = tustr::is_collect_regex_back_slash("a\\0\\\\a\\\\");
    constexpr auto case2 = tustr::is_collect_regex_back_slash("a\\0\\\\a\\");
    constexpr auto case3 = tustr::is_collect_regex_back_slash("\\a\\0\\\\a\\\\");
    constexpr auto case4 = tustr::is_collect_regex_back_slash("a\\0\\\\\\a\\\\");
    constexpr auto case5 = is_allowed_string("aaaab", true, "ab", "");
    constexpr auto case6 = is_allowed_string("\\0\\n\\\\", true, "", "0n\\");
    constexpr auto case7 = is_allowed_string("a\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case8 = is_allowed_string("a\\0\\\\a\\\\", false, "bc", "ds");
    constexpr auto case9 = is_allowed_string("ab\\0\\\\a\\\\", true, "a", "\\0");
    constexpr auto case10 = is_allowed_string("a\\d\\0\\\\a\\\\", false, "bc", "ds");
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

TEST(tustrcpptest, RegexExtractBrancketTest)
{
    // 正常
    constexpr auto case11 = tustr::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)]rs", 3>::value;
    constexpr auto case12 = tustr::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)}rs", 3>::value;
    constexpr auto case13 = tustr::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\)))rs", 3>::value;
    constexpr auto case14 = tustr::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)>rs", 3>::value;
    EXPECT_STREQ(case11.data(), "defg{hijk}lm\\](opq)");
    EXPECT_STREQ(case12.data(), "defg\\{hijk\\}lm](opq)");
    EXPECT_STREQ(case13.data(), "defg{hijk\\}lm](opq\\))");
    EXPECT_STREQ(case14.data(), "defg{hijk\\}lm](opq\\)");

    // 異常(対応する閉じ括弧が存在しない) コメントアウトを外すとコンパイルエラーが発生
    // constexpr auto case15 = tustr::regex_bracket_inner<"abc[defg{hijk}lm\\](opq)\\]rs", 3>::value;
    // constexpr auto case16 = tustr::regex_bracket_inner<"abc{defg\\{hijk\\}lm](opq)\\}rs", 3>::value;
    // constexpr auto case17 = tustr::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\)\\)rs", 3>::value;
    // constexpr auto case18 = tustr::regex_bracket_inner<"abc<defg{hijk\\}lm](opq\\)\\>rs", 3>::value;
    // constexpr auto case19 = tustr::regex_bracket_inner<"abc(defg{hijk\\}lm](opq\\))rs", 3>::value;
}

/**
 * @fn
 * @brief 渡された文字範囲を許可/拒否リストに展開し、与えられた文字列がマッチするか判定する関数オブジェクトを返却する
 * TODO: いらないのでいつかテストの修正と本関数を削除する
*/
template <tustr::cstr Pattern, std::size_t Pos>
static constexpr auto get_regex_char_range_matcher()
{
    return [](const std::string& comp) -> bool {
        using range_parser = tustr::regex_char_set_parser<Pattern, Pos>;
        return is_allowed_string(
            comp,
            range_parser::allow_or_deny,
            range_parser::value.data(),
            range_parser::bk_value.data(),
            false
        );
    };
}

TEST(tustrcpptest, RegexCharRangeParseTest)
{
    // 文字クラス,バックスラッシュ,文字範囲指定なし
    constexpr auto case19 = get_regex_char_range_matcher<"ab[-abc.]", 2>()(".c-aba");
    constexpr auto case20 = get_regex_char_range_matcher<"[abc.-]", 0>()(".c-aba");
    constexpr auto case21 = get_regex_char_range_matcher<"[-abc.]", 0>()("b.c-d");
    constexpr auto case22 = get_regex_char_range_matcher<"[-abc.]", 0>()("def");
    constexpr auto case23 = get_regex_char_range_matcher<"[^-abc]", 0>()("cab");
    constexpr auto case24 = get_regex_char_range_matcher<"[^-abc]", 0>()("bcd");
    constexpr auto case25 = get_regex_char_range_matcher<"[^-abc]", 0>()("def^");
    EXPECT_TRUE(case19);
    EXPECT_TRUE(case20);
    EXPECT_FALSE(case21);
    EXPECT_FALSE(case22);
    EXPECT_FALSE(case23);
    EXPECT_FALSE(case24);
    EXPECT_TRUE(case25);

    // 文字クラス指定あり
    constexpr auto case26 = get_regex_char_range_matcher<"[\\d]", 0>()("0123456789");
    constexpr auto case27 = get_regex_char_range_matcher<"[\\d]", 0>()("0123456789a");
    constexpr auto case28 = get_regex_char_range_matcher<"[^\\d]", 0>()("0aabc_%&\t\\[");
    constexpr auto case29 = get_regex_char_range_matcher<"[^\\d]", 0>()("abc_%&\t\\[");
    constexpr auto case30 = get_regex_char_range_matcher<"[\\D]", 0>()("0aabc_%&\t\\[");
    constexpr auto case31 = get_regex_char_range_matcher<"[\\D]", 0>()("aabc_%&\t\\[");
    constexpr auto case32 = get_regex_char_range_matcher<"[^\\D]", 0>()("0123456789");
    constexpr auto case33 = get_regex_char_range_matcher<"[^\\D]", 0>()("0123456789a");

    // 生成した関数を保持できるかも確認(レンジのテスト)
    constexpr auto range_matcher1 = get_regex_char_range_matcher<"[0-5a-dA\\-E\\t]", 0>();

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

TEST(tustrcpptest, RegexQuantifierParseTest)
{
    using type1 = tustr::regex_quantifier_perser<"abcd*e", 4>;
    using type2 = tustr::regex_quantifier_perser<"abcd*", 4>;
    using type3 = tustr::regex_quantifier_perser<"abcd*?", 4>;
    using type4 = tustr::regex_quantifier_perser<"abcd+", 4>;
    using type5 = tustr::regex_quantifier_perser<"abcd?", 4>;
    using type6 = tustr::regex_quantifier_perser<"abcd{20}e", 4>;
    using type7 = tustr::regex_quantifier_perser<"abcd{2,}", 4>;
    using type8 = tustr::regex_quantifier_perser<"abcd{25,512}", 4>;
    using type9 = tustr::regex_quantifier_perser<"abcd{20}?e", 4>;
    using type10 = tustr::regex_quantifier_perser<"abcd{2,}?", 4>;
    using type11 = tustr::regex_quantifier_perser<"abcd{25,512}?", 4>;

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
    EXPECT_EQ(type6::min_count, 20);
    EXPECT_EQ(type6::max_count, 20);
    EXPECT_EQ(type7::min_count, 2);
    EXPECT_EQ(type7::max_count, std::string_view::npos);
    EXPECT_EQ(type8::min_count, 25);
    EXPECT_EQ(type8::max_count, 512);

    EXPECT_EQ(type1::begin_pos, 4);
    EXPECT_EQ(type1::end_pos, 5);
    EXPECT_EQ(type3::begin_pos, 4);
    EXPECT_EQ(type3::end_pos, 6);
    EXPECT_EQ(type4::begin_pos, 4);
    EXPECT_EQ(type4::end_pos, 5);
    EXPECT_EQ(type11::begin_pos, 4);
    EXPECT_EQ(type11::end_pos, 13);
}

TEST(tustrcpptest, RegexCaptureParserTest)
{
    using type1 = tustr::regex_capture_parser<"ab(cdefg)", 2>;
    using type2 = tustr::regex_capture_parser<"ab(?:cdefg)", 2>;
    using type3 = tustr::regex_capture_parser<"ab(?<a_1>cdefg)", 2>;
    using type4 = tustr::regex_capture_parser<"abcdef((ghi[jkl].){2,4}(\\d(\\]m))){2}(aa)\\)nop", 6>;

    EXPECT_TRUE(type1::is_capture);
    EXPECT_FALSE(type2::is_capture);
    EXPECT_TRUE(type3::is_capture);
    EXPECT_STREQ(type1::name.data(), "");
    EXPECT_STREQ(type2::name.data(), "");
    EXPECT_STREQ(type3::name.data(), "a_1");
    EXPECT_FALSE(type1::is_named);
    EXPECT_FALSE(type2::is_named);
    EXPECT_TRUE(type3::is_named);
    EXPECT_STREQ(type1::capture_pattern.data(), "cdefg");
    EXPECT_STREQ(type2::capture_pattern.data(), "cdefg");
    EXPECT_STREQ(type3::capture_pattern.data(), "cdefg");
    EXPECT_STREQ(type4::capture_pattern.data(), "(ghi[jkl].){2,4}(\\d(\\]m))");
    EXPECT_EQ(type4::inner_regex::max_capture_count, 6);

    EXPECT_TRUE(tustr::RegexParserCaptureable<type1>);
    EXPECT_FALSE(tustr::RegexParserCaptureable<type2>);
    EXPECT_TRUE(tustr::RegexParserCaptureable<type3>);
}

TEST(tustrcpptest, RegexGeneralTest)
{
    using type1 = tustr::regex_general<"abcdef[g", 0>;
    EXPECT_STREQ(type1::value.data(), "abcdef");
    EXPECT_EQ(type1::begin_pos, 0);
    EXPECT_EQ(type1::end_pos, 6);
}

TEST(tustrcpptest, RegexReferenceTest)
{
    using type1 = tustr::regex_reference_parser<"\\1", 1>;
    using type2 = tustr::regex_reference_parser<"\\345", 1>;

    EXPECT_EQ(type1::end_pos, 2);
    EXPECT_EQ(type1::reference_index, 0);
    EXPECT_EQ(type2::end_pos, 4);
    EXPECT_EQ(type2::reference_index, 344);
}

TEST(tustrcpptest, RegexAddQuantifierTest)
{
    using type1 = tustr::add_quantifier<"abcdef[g", tustr::regex_general<"abcdef[g", 0>>::type;
    using type2 = tustr::add_quantifier<"abcdef+[g", tustr::regex_general<"abcdef+[g", 0>>::type;
    using type3 = tustr::add_quantifier<"abcdef+[g", tustr::regex_general<"abcdef+[g", 5>>::type;

    EXPECT_EQ(type1::begin_pos, 0);
    EXPECT_EQ(type1::end_pos, 6);
    EXPECT_EQ(type2::begin_pos, 0);
    EXPECT_EQ(type2::end_pos, 5);
    EXPECT_EQ(type3::begin_pos, 5);
    EXPECT_EQ(type3::end_pos, 7);
    EXPECT_EQ(type3::min_count, 1);
    EXPECT_EQ(type3::max_count, std::string_view::npos);
}

TEST(tustrcpptest, RegexAssertionTest)
{

    constexpr auto check_func = [](auto t, std::string_view sv, std::size_t offset, bool is_lock_pos) {
        tustr::regex_capture_store<0> cs;
        return t.generated_func<0>(sv, offset, is_lock_pos, cs).get_begin_pos();
    };

    using type1 = tustr::regex_assertion_parser<"^", 0>;
    EXPECT_EQ(check_func(type1{}, "", 0, true), 0);
    EXPECT_EQ(check_func(type1{}, "a", 0, true), 0);
    EXPECT_EQ(check_func(type1{}, "a", 1, true), std::string_view::npos);
    EXPECT_EQ(check_func(type1{}, "a", 2, true), std::string_view::npos);
    EXPECT_EQ(check_func(type1{}, "a\n", 1, true), std::string_view::npos);
    EXPECT_EQ(check_func(type1{}, "a\n", 1, false), 2);
    EXPECT_EQ(check_func(type1{}, "a\n", 2, true), 2);
    EXPECT_EQ(check_func(type1{}, "\na", 1, true), 1);
    EXPECT_EQ(check_func(type1{}, "a\na", 1, false), 2);

    using type2 = tustr::regex_assertion_parser<"$", 0>;
    EXPECT_EQ(check_func(type2{}, "", 0, true), 0);
    EXPECT_EQ(check_func(type2{}, "a", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type2{}, "a", 0, false), 1);
    EXPECT_EQ(check_func(type2{}, "a", 1, true), 1);
    EXPECT_EQ(check_func(type2{}, "a", 2, true), std::string_view::npos);
    EXPECT_EQ(check_func(type2{}, "a\n", 1, true), 1);
    EXPECT_EQ(check_func(type2{}, "a\n", 0, false), 1);
    EXPECT_EQ(check_func(type2{}, "\na", 0, true), 0);
    EXPECT_EQ(check_func(type2{}, "\na", 1, true), std::string_view::npos);

    using type3 = tustr::regex_assertion_parser<"b", 0>;
    EXPECT_EQ(check_func(type3{}, "", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type3{}, "a", 0, true), 0);
    EXPECT_EQ(check_func(type3{}, "a", 1, true), 1);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 2, true), std::string_view::npos);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 3, true), 3);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 4, true), 4);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 7, true), 7);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 8, true), 8);
    EXPECT_EQ(check_func(type3{}, "ab_@12d xy", 1, false), 3);

    using type4 = tustr::regex_assertion_parser<"B", 0>;
    EXPECT_EQ(check_func(type4{}, "", 0, true), 0);
    EXPECT_EQ(check_func(type4{}, "a", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "a", 1, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 2, true), 2);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 3, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 4, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 7, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 8, true), std::string_view::npos);
    EXPECT_EQ(check_func(type4{}, "ab_@12d xy", 7, false), 9);

    using type5 = tustr::regex_assertion_parser<"(?=abc)", 0>;
    EXPECT_STREQ(type5::inner_match_pattern.data(), "abc");
    EXPECT_EQ(check_func(type5{}, "ab", 0, false), std::string_view::npos);
    EXPECT_EQ(check_func(type5{}, "abc", 0, true), 0);
    EXPECT_EQ(check_func(type5{}, "abc", 3, false), std::string_view::npos); // 境界(文字列の最後尾の位置を指定)
    EXPECT_EQ(check_func(type5{}, "aaabc", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type5{}, "aaabc", 0, false), 2);

    using type6 = tustr::regex_assertion_parser<"(?!abc)", 0>;
    EXPECT_STREQ(type6::inner_match_pattern.data(), "abc");
    EXPECT_EQ(check_func(type6{}, "ab", 0, false), 0);
    EXPECT_EQ(check_func(type6{}, "abc", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type6{}, "abc", 3, true), 3); // 境界(文字列の最後尾の位置を指定)
    EXPECT_EQ(check_func(type6{}, "aaabc", 0, true), 0);
    EXPECT_EQ(check_func(type6{}, "aaabc", 1, false), 1);
    EXPECT_EQ(check_func(type6{}, "aaabc", 2, false), 3);
    EXPECT_EQ(check_func(type6{}, "aaabc", 2, true), std::string_view::npos);

    using type7 = tustr::regex_assertion_parser<"(?<=abc)", 0>;
    EXPECT_STREQ(type7::inner_match_pattern.data(), "abc");
    EXPECT_EQ(check_func(type7{}, "ab", 0, false), std::string_view::npos);
    EXPECT_EQ(check_func(type7{}, "abc", 0, false), 3);
    EXPECT_EQ(check_func(type7{}, "abc", 0, true), std::string_view::npos);
    EXPECT_EQ(check_func(type7{}, "abc", 3, true), 3);
    EXPECT_EQ(check_func(type7{}, "abcc", 0, false), 3);
    EXPECT_EQ(check_func(type7{}, "abcc", 3, true), 3);
    EXPECT_EQ(check_func(type7{}, "aabc", 0, false), 4);

    using type8 = tustr::regex_assertion_parser<"(?<!abc)", 0>;
    EXPECT_STREQ(type8::inner_match_pattern.data(), "abc");
    EXPECT_EQ(check_func(type8{}, "ab", 0, false), 0);
    EXPECT_EQ(check_func(type8{}, "abc", 0, false), 0);
    EXPECT_EQ(check_func(type8{}, "abc", 3, false), std::string_view::npos);
    EXPECT_EQ(check_func(type8{}, "abcc", 3, false), 4);
    EXPECT_EQ(check_func(type8{}, "abcc", 3, true), std::string_view::npos);
}

TEST(tustrcpptest, RegexParseTest)
{
    using type1 = tustr::regex<"abcdef[ghi].\\daz[$%&_1]\\[+\\^?">;
    using type2 = tustr::regex<"abcdef(ghi[jkl].\\d\\]m){2}\\)nop">;
    using type3 = tustr::regex<"abcdef((ghi[jkl].){2,4}(\\d(\\]m))){2}(aa)\\)nop">;
    using type4 = tustr::regex<"abcdef(ghi[jkl].\\d\\]m){2,}\\)nop">;

    EXPECT_EQ(type1::max_capture_count, 0);
    EXPECT_EQ(type2::max_capture_count, 2);
    EXPECT_EQ(type3::max_capture_count, 15);
    EXPECT_EQ(type4::max_capture_count, type4::allowed_max_capture_count);

    constexpr auto f_arr = type1::match_rules;
    // {'abcdef', '[ghi]', '.', '\\d', 'az', '[$%&_1]', '\\[+', '\\^?']}それぞれに対して8つの関数が生成される
    ASSERT_EQ(f_arr.size(), 8);

    constexpr auto check_func1 = [](auto f, std::string_view sv, std::size_t offset, bool is_fixed) {
        using capture_store_type1 = tustr::regex_capture_store<type1::max_capture_count>;
        capture_store_type1 dummy_cs{};
        return f(sv, offset, is_fixed, dummy_cs).get_end_pos();
    };

    constexpr auto case1 = check_func1(f_arr[0], "abgbzabcdefrrr", 0, false);
    constexpr auto case2 = check_func1(f_arr[1], "abgbzabcdefrrr", 0, false);
    constexpr auto case3 = check_func1(f_arr[1], "abgbzabcdefrrr", 2, false);
    constexpr auto case4 = check_func1(f_arr[2], "a\\%1", 0, false);
    constexpr auto case5 = check_func1(f_arr[2], "a\\%1", 1, false);
    constexpr auto case6 = check_func1(f_arr[2], "a\\%1", 2, false);
    constexpr auto case7 = check_func1(f_arr[2], "a\\%1", 3, false);
    constexpr auto case8 = check_func1(f_arr[3], "a\\%1", 0, false);
    constexpr auto case9 = check_func1(f_arr[3], "a\\%1", 1, false);
    constexpr auto case10 = check_func1(f_arr[3], "a\\%1", 3, false);

    EXPECT_EQ(case1, 5 + tustr::cstr{"abcdef"}.size());
    EXPECT_EQ(case2, 2 + 1);
    EXPECT_EQ(case3, 2 + 1);
    EXPECT_EQ(case4, 0 + 1);
    EXPECT_EQ(case5, 1 + 1);
    EXPECT_EQ(case6, 2 + 1);
    EXPECT_EQ(case7, 3 + 1);
    EXPECT_EQ(case8, std::string_view::npos);
    EXPECT_EQ(case9, std::string_view::npos);
    EXPECT_EQ(case10, 3 + 1);

    constexpr auto case11 = check_func1(f_arr[0], "abgbzabcdefrrr", 0, true);
    constexpr auto case12 = check_func1(f_arr[0], "abgbzabcdefrrr", 5, true);
    constexpr auto case13 = check_func1(f_arr[1], "abgbzabcdefrrr", 0, true);
    constexpr auto case14 = check_func1(f_arr[1], "abgbzabcdefrrr", 2, true);

    EXPECT_EQ(case11, std::string_view::npos);
    EXPECT_EQ(case12, 5 + tustr::cstr{"abcdef"}.size());
    EXPECT_EQ(case13, std::string_view::npos);
    EXPECT_EQ(case14, 2 + 1);

    constexpr auto f_arr2 = type2::match_rules;
    ASSERT_EQ(f_arr2.size(), 3);

    constexpr auto check_func2 = [](auto f, std::string_view sv, std::size_t offset, bool is_fixed) {
        using capture_store_type2 = tustr::regex_capture_store<type2::max_capture_count>;
        capture_store_type2 dummy_cs{};
        return f(sv, offset, is_fixed, dummy_cs).get_end_pos();
    };
    
    constexpr auto case15 = check_func2(f_arr2[1], "ghij%1]m", 0, false);
    constexpr auto case16 = check_func2(f_arr2[1], "ghij%1]mghil<9]m", 0, false);
    constexpr auto case17 = check_func2(f_arr2[1], "aaghij%1]mghil<9]mghik#0]m", 0, false);

    EXPECT_EQ(case15, std::string_view::npos);
    EXPECT_EQ(case16, 16);
    EXPECT_EQ(case17, 18);
}

TEST(tustrcpptest, RegexRunTest)
{
    using type1 = tustr::regex<"abcdef[ghi].\\daz[$%&_1]\\[+\\^?">;
    using type2 = tustr::regex<"abcdef(ghi[jkl].\\d\\]m){2}\\)nop">;
    using type3 = tustr::regex<"abcdef((ghi[jkl].){2,4}(\\d(\\]m))){2}(aa)\\)nop">;
    using type4 = tustr::regex<"(?=.{0,4}[A-Z]).{5}">;

    constexpr auto case1 = type1::run("abcdefg#5az&[^");
    constexpr auto case2 = type1::run("abcdefv#5az&[^");
    constexpr auto case3 = type1::run("nnnabcdefg#5az&[^nnn");
    constexpr auto case4 = type1::run("nnnabcdefng#5az&[^nnn");
    constexpr auto case5 = type1::run("abcdefg#5az&[[[");
    constexpr auto case6 = type1::run("abcdefg#5az&[^^");
    constexpr auto case7 = type1::run("abcdefg#5az&^^");

    EXPECT_EQ(case1.second.get_end_pos(), 14);
    EXPECT_EQ(case2.second.get_end_pos(), std::string_view::npos);
    EXPECT_EQ(case3.second.get_end_pos(), 17);
    EXPECT_EQ(case4.second.get_end_pos(), std::string_view::npos);
    EXPECT_EQ(case5.second.get_end_pos(), 15);
    EXPECT_EQ(case6.second.get_end_pos(), 14);
    EXPECT_EQ(case7.second.get_end_pos(), std::string_view::npos);

    // グループ二回繰り返し
    constexpr auto case8 = type2::run("abcdefghij%1]mghil<9]m)nop");
    constexpr auto case9 = type2::run("nnnabcdefghij%1]mghil<9]m)nopnnn");
    // グループ一回繰り返し
    constexpr auto case10 = type2::run("abcdefghij%1]m)nop");
    // グループ三回繰り返し
    constexpr auto case11 = type2::run("abcdefghij%1]mghil<9]mghik#0]m)nop");

    EXPECT_EQ(case8.second.get_end_pos(), 26);
    EXPECT_EQ(case9.second.get_end_pos(), 29);
    EXPECT_EQ(case10.second.get_end_pos(), std::string_view::npos);
    EXPECT_EQ(case11.second.get_end_pos(), std::string_view::npos);

    EXPECT_EQ(case8.first.get(0), "ghij%1]m"sv);
    EXPECT_EQ(case8.first.get(1), "ghil<9]m"sv);

    constexpr auto case12 = type3::run("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nop");

    EXPECT_EQ(case12.second.get_end_pos(), 43);
    ASSERT_EQ(case12.first.size(), 12);
    EXPECT_EQ(case12.first.get(0), "ghij@ghij$ghij%1]m"sv);
    EXPECT_EQ(case12.first.get(1), "ghij@"sv);
    EXPECT_EQ(case12.first.get(2), "ghij$"sv);
    EXPECT_EQ(case12.first.get(3), "ghij%"sv);
    EXPECT_EQ(case12.first.get(4), "1]m"sv);
    EXPECT_EQ(case12.first.get(5), "]m"sv);
    EXPECT_EQ(case12.first.get(6), "ghij/ghij|2]m"sv);
    EXPECT_EQ(case12.first.get(7), "ghij/"sv);
    EXPECT_EQ(case12.first.get(8), "ghij|"sv);
    EXPECT_EQ(case12.first.get(9), "2]m"sv);
    EXPECT_EQ(case12.first.get(10), "]m"sv);
    EXPECT_EQ(case12.first.get(11), "aa"sv);

    constexpr auto case13 = type4::run("abcde1234f");
    constexpr auto case14 = type4::run("aBcde1234f");
    constexpr auto case15 = type4::run("abcdE1234f");
    constexpr auto case16 = type4::run("abcde1234F");

    EXPECT_EQ(case13.second.get_begin_pos(), std::string_view::npos);
    // EXPECT_EQ(case14.second.get_begin_pos(), 0); // 繰り返し終了位置の判定がおかしいため、修正すること
    EXPECT_EQ(case15.second.get_begin_pos(), 0);
    EXPECT_EQ(case16.second.get_begin_pos(), 5);
}

TEST(tustrcpptest, RegexMatchTest)
{
    using type1 = tustr::regex<"abcdef((ghi[jkl].){2,4}(\\d(\\]m))){2}(aa)\\)nop\\2\\6">;
    constexpr auto case1 = type1::search("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case2 = type1::search("aabcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case3 = type1::search("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]ma");
    constexpr auto case4 = type1::match("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case5 = type1::match("aabcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case6 = type1::match("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]ma");

    EXPECT_TRUE(case1);
    EXPECT_TRUE(case2);
    EXPECT_TRUE(case3);
    EXPECT_TRUE(case4);
    EXPECT_FALSE(case5);
    EXPECT_FALSE(case6);
}

TEST(tustrcpptest, RegexResultTest)
{
    using type1 = tustr::regex<"abcdef((ghi[jkl].){2,4}(\\d(\\]m))){2}(aa)\\)nop\\2\\6">;
    constexpr auto case1 = type1("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case2 = type1("aabcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m");
    constexpr auto case3 = type1("abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]ma");
    constexpr auto case4 = type1("abcefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]ma");

    EXPECT_TRUE(case1.is_find());
    EXPECT_TRUE(case2.is_find());
    EXPECT_TRUE(case3.is_find());
    EXPECT_FALSE(case4.is_find());
    EXPECT_TRUE(case1.is_match());
    EXPECT_FALSE(case2.is_match());
    EXPECT_FALSE(case3.is_match());
    EXPECT_FALSE(case4.is_match());
    EXPECT_EQ(case1.get_match_part(), "abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m"sv);
    EXPECT_EQ(case2.get_match_part(), "abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m"sv);
    EXPECT_EQ(case3.get_match_part(), "abcdefghij@ghij$ghij%1]mghij/ghij|2]maa)nopghij@]m"sv);
    EXPECT_EQ(case4.get_match_part(), ""sv);

    EXPECT_EQ(case1.get_match_part(2), "ghij@"sv);
    EXPECT_EQ(case1.get_match_part(7), "ghij/ghij|2]m"sv);
    EXPECT_EQ(case1.get_match_part(12), "aa"sv);
    EXPECT_EQ(case1.get_match_part(13), ""sv);
    EXPECT_EQ(case1.get_match_part(99), ""sv);
}
