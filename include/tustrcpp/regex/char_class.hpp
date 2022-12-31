///----------------------------------
/// @file char_class.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_CHAR_CLASS_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_CHAR_CLASS_HPP

namespace tustr
{
    struct regex_char_class
    {
    private:
        static constexpr auto digits            = cstr{"0123456789"};
        static constexpr auto words             = cstr{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"};
        static constexpr auto space             = cstr{"\f\t\v"};
        static constexpr auto strends           = cstr{"\n\r"};
        static constexpr auto bk_space          = cstr{"ftv"};
        static constexpr auto bk_strends        = cstr{"nr"};
        static constexpr auto others            = cstr{"-!\"#$%&'()=^~|`@{}+;*:,<.>/?\b\\"};
        static constexpr auto bk_others         = cstr{"b0[]\\"};

        static constexpr auto true_words        = concat(digits, words);
        static constexpr auto true_space        = concat(space, strends);
        static constexpr auto bk_true_space     = concat(bk_space, bk_strends);
        static constexpr auto dot               = concat(digits, words, space, others);
        static constexpr auto bk_dot            = concat(bk_space, bk_others);
        static constexpr auto anti_digits       = concat(words, space, strends, others);
        static constexpr auto bk_anti_digits    = concat(bk_space, bk_strends, bk_others);
        static constexpr auto anti_words        = concat(space, strends, others);
        static constexpr auto bk_anti_words     = concat(bk_space, bk_others, bk_others);
        static constexpr auto anti_space        = concat(digits, words, others);

    public:
        // 特殊文字に該当する文字集合を取得する
        inline static constexpr auto get_char_set(const char ch)
        {
            switch (ch) {
                case 'd': return std::pair{digits.view(), std::string_view{}};
                case 'w': return std::pair{true_words.view(), std::string_view{}};
                case 's': return std::pair{true_space.view(), bk_true_space.view()};
                case '.': return std::pair{dot.view(), bk_dot.view()};
                case 'D': return std::pair{anti_digits.view(), bk_anti_digits.view()};
                case 'W': return std::pair{anti_words.view(), bk_anti_words.view()};
                case 'S': return std::pair{anti_space.view(), bk_others.view()};
            }
            return std::pair{std::string_view{}, std::string_view{}};
        }

        template <char C>
        static constexpr auto get_const_char_set()
        {
            if constexpr (C == 'd') return digits;
            else if constexpr (C == 'w') return true_words;
            else if constexpr (C == 's') return true_space;
            else if constexpr (C == '.') return dot;
            else if constexpr (C == 'D') return anti_digits;
            else if constexpr (C == 'W') return anti_words;
            else if constexpr (C == 'S') return anti_space;
            else if constexpr (C == 'f') return cstr{"\f"};
            else if constexpr (C == 't') return cstr{"\t"};
            else if constexpr (C == 'v') return cstr{"\v"};
            else if constexpr (C == 'n') return cstr{"\n"};
            else if constexpr (C == 'r') return cstr{"\r"};
            else if constexpr (C == 'b') return cstr{"\b"};
            else return char_to_cstr(C);
        }

        template <char C>
        static constexpr auto get_const_bk_char_set()
        {
            if constexpr (C == 'd') return cstr{""};
            else if constexpr (C == 'w') return cstr{""};
            else if constexpr (C == 's') return bk_true_space;
            else if constexpr (C == '.') return bk_dot;
            else if constexpr (C == 'D') return bk_anti_digits;
            else if constexpr (C == 'W') return bk_anti_words;
            else if constexpr (C == 'S') return bk_others;
            else return char_to_cstr(C);
        }
    };

    template <cstr Pattern, std::size_t Pos>
    struct regex_char_class_parser
    {
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = Pos + 1;
        static constexpr auto value = regex_char_class::get_const_char_set<Pattern[Pos]>();

        /**
         * @fn
         * @brief 解析結果生成された処理
         * TODO: is_pos_lockがfalseの場合でもtrueと同様の動作をしてしまう？関連テスト追加と修正を行うこと
        */
        template <std::size_t N>
        static constexpr regex_match_range generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            return offset < s.size() && value.contains(char_to_cstr(s[offset]))
                ? regex_match_range{offset, 1}
                : regex_match_range::make_unmatch();
        }
    };
}

#endif
