///----------------------------------
/// @file str/regex/reference.hpp 
/// @brief regex.hpp関連(後方参照)
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP

namespace tuutil::str::_regex
{
    /**
     * @brief インデックスによるキャプチャの後方参照
     * @tparam Pattern 正規表現パターンの固定長文字列
     * @tparam Pos 解析開始位置
    */
    template <cstr Pattern, std::size_t Pos>
    struct reference_parser
    {
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = Pos + Pattern.get_charset_match_range("0123456789", Pos)[1];
        static constexpr auto reference_index = to_int<std::size_t>(Pattern.template substr<Pos, end_pos - Pos>().view()) - 1;

        /**
         * @fn
         * @brief 既にキャプチャが存在することが前提のため、is_pos_lockの真偽に関わらず位置は常に固定でOK
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto search_str = cs.get(reference_index);
            return exists_in_position(search_str, s, offset)
                ? regex_match_result{offset, search_str.size()}
                : regex_match_result::make_unmatch();
        }
    };

    /**
     * @brief 名前付きキャプチャの後方参照
     * @tparam Pattern 正規表現パターンの固定長文字列
     * @tparam Pos 解析開始位置
    */
    template <cstr Pattern, std::size_t Pos>
    requires (exists_in_position("k<", Pattern, Pos))
    struct reference_parser<Pattern, Pos>
    {
        static constexpr auto begin_pos = Pos;
        using named_bracket = bracket_inner<Pattern, Pos + 1>;
        static constexpr auto reference_name = named_bracket::value;
        static constexpr auto end_pos = Pos + 1 + named_bracket::value_with_bracket.size();

        // キャプチャ名は一文字以上かつ、半角英数字_のみ指定可能としてそれ以外はコンパイルエラー
        static_assert(
            reference_name.size() > 0 && reference_name.match_all_charset(char_class::get_const_char_set<'w'>()),
            "An error has occurred. [named reference_parser]"
        );

        /**
         * @fn
         * @brief 既にキャプチャが存在することが前提のため、is_pos_lockの真偽に関わらず位置は常に固定でOK
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto search_str = cs.get(reference_name.view());
            return exists_in_position(search_str, s, offset)
                ? regex_match_result{offset, search_str.size()}
                : regex_match_result::make_unmatch();
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP
