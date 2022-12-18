///----------------------------------
/// @file regex/general.hpp 
/// @brief regex.hppの関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_GENERAL_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_GENERAL_HPP

namespace tustr
{
    /**
     * @class
     * @brief 通常の文字列のマッチング(とじ括弧のような単体で存在しない者の検証もここで)
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_general
        : public regex_char_attribute
    {
        static constexpr auto begin_pos = Pos;

        static constexpr auto end_pos = []() {
            for (auto i = Pos; i < Pattern.size(); i++)
                if (attributes[Pattern[i]] && !(attributes[Pattern[i]] & BK)) {
                    // 数量詞は直前の１文字のみに適用されるため、一気に判定する文字列からは除外する
                    if (i - Pos > 1 && (attributes[Pattern[i]] & QUANTIFIER)) return i - 1;
                    return i;
                }
            return Pattern.size();
        }();

        static constexpr auto value = Pattern.substr<Pos, end_pos - Pos>();

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        static constexpr std::size_t generated_func(const std::string_view& s, std::size_t offset, bool is_pos_lock)
        {
            auto pos = is_pos_lock
                ? (exists_in_position(value, s, offset) ? offset : std::string_view::npos)
                : find(value, s, offset);
            if (pos != std::string_view::npos) pos += value.size();
            return pos;
        }
    };
}

#endif //TUSTRCPP_INCLUDE_GUARD_REGEX_GENERAL_HPP
