///----------------------------------
/// @file regex/general.hpp 
/// @brief regex.hppの関連
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_OR_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_OR_HPP

namespace tuutil::str::_regex
{
    /**
     * @fn
     * @brief 正規表現パターンより、「|」を検索する。()や[]等の中は検索対象外で見つからない場合はnposを返す
    */
    inline constexpr auto get_or_pos_regex_pattern(std::string_view pattern, std::size_t offset = 0)
    {
        // 括弧の多重度で対応括弧か判定
        for (std::size_t depth{}; const auto i : std::views::iota(offset, pattern.size()))
            if (!eq_before_char(pattern, i, '\\'))
                switch (pattern[i]) {
                    case '[':
                    case '(':
                    case '{':
                        depth++;
                        break;
                    case ']':
                    case ')':
                    case '}':
                        depth--;
                        break;
                    case '|':
                        if (depth == 0) return i;
                        break;
                }
        // 未発見
        return std::string_view::npos;
    }

    /**
     * @class
     * @brief 「|」で分割されたパターンをそれぞれ正規表現とし、いずれか一つでもマッチすれば真とするよう、処理を解析する
    */
    template <cstr Pattern, std::size_t Pos>
    struct or_parser
    {
        static_assert(Pattern[Pos] == '|');
        static constexpr std::size_t begin_pos = 0;
        static constexpr std::size_t end_pos = Pattern.size();
        static constexpr std::size_t min_count = 1;
        static constexpr std::size_t max_count = 1;
        static constexpr auto pref_str = Pattern.template remove_suffix<end_pos - Pos>();
        static constexpr auto suf_str = Pattern.template remove_prefix<Pos + 1>();
        using pref_regex = regex<pref_str, regex_parser>;
        using suf_regex = regex<suf_str, regex_parser>;
        using inner_regex = std::conditional_t<
            (pref_regex::parser::max_capture_count > suf_regex::parser::max_capture_count),
            pref_regex,
            suf_regex
        >;

        /**
         * @fn
         * @brief 解析結果生成された処理(csの更新もいる？)
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto [cs1, re1] = pref_regex::exec(s, offset, is_pos_lock);
            if (re1) {
                cs.push_back(cs1);
                return re1;
            }
            const auto [cs2, re2] = suf_regex::exec(s, offset, is_pos_lock);
            if (re2) {
                cs.push_back(cs2);
                return re2;
            }
            return regex_match_result::make_unmatch();
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_OR_HPP
