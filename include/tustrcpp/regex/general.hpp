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
                if (attributes[Pattern[i]] && !(attributes[Pattern[i]] & BK)) return i;
            return Pattern.size();
        }();

        static_assert(
            end_pos == Pattern.size() || !(attributes[Pattern[end_pos]] & DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

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
