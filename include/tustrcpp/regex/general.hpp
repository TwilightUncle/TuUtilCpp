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
            end_pos == Pattern.size() || !deny_chars.contains(char_to_cstr(Pattern[end_pos])),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

        static constexpr auto value = Pattern.substr<Pos, end_pos - Pos>();
    };
}

#endif //TUSTRCPP_INCLUDE_GUARD_REGEX_GENERAL_HPP
