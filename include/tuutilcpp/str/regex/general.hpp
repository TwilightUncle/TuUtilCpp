///----------------------------------
/// @file regex/general.hpp 
/// @brief regex.hppの関連
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief 通常の文字列のマッチング(とじ括弧のような単体で存在しない者の検証もここで)
    */
    template <cstr Pattern, std::size_t Pos>
    struct general
        : public regex_char_attribute
    {
        static constexpr auto begin_pos = Pos;

        static constexpr auto end_pos = []() {
            for (auto i = Pos; i < Pattern.size(); i++) {
                // 先頭が特殊文字の時はエスケープされていると想定
                if (attributes[Pattern[i]] && i == Pos) continue;

                // 二文字目以降の特殊文字
                if (attributes[Pattern[i]] && !(attributes[Pattern[i]] & BK)) {
                    // 数量詞は直前の１文字のみに適用されるため、一気に判定する文字列からは除外する
                    if (i - Pos > 1 && (attributes[Pattern[i]] & QUANTIFIER)) return i - 1;
                    return i;
                }
            }
            return Pattern.size();
        }();

        static constexpr auto value = Pattern.template substr<Pos, end_pos - Pos>();

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            auto pos = is_pos_lock
                ? (exists_in_position(value, s, offset) ? offset : std::string_view::npos)
                : find(value, s, offset);
            return (pos == std::string_view::npos)
                ? regex_match_result::make_unmatch()
                : regex_match_result{pos, value.size()};
        }
    };
}

#endif //TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP
