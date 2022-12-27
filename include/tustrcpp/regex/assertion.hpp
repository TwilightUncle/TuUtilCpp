///----------------------------------
/// @file assertion.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_ASSERTION_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_ASSERTION_HPP

namespace tustr
{
    /**
     * @fn
     * @brief 境界assertionについて解析し、判定関数を生成する
     * キャプチャと同等に再帰的にパターン解析し、境界アサーションは文字列マッチングに置き換えて実行する方向で考える
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_assertion_boundary_parser
    {
        static constexpr auto begin_pos = Pos;

        enum check_flags
        {
            BEGIN           = 0x0001u,  // 対象文字列の先頭にマッチ
            END             = 0x0002u,  // 対象文字列の末尾にマッチ
            EMPTY           = 0x0004u,  // 空文字列に対してマッチ
            WORD_BOUNDARY   = 0x0008u,  // 単語の境界
            IN_WORD         = 0x0010u,  // 単語内
            LOOK_AHEAD      = 0x0020u,  // 先読み
            LOOK_BEHIND     = 0x0040u,  // 後読み
            NEGATIVE        = 0x0080u,  // 否定的の場合立てる
        };

        static constexpr auto check_flag = []() -> std::size_t {
            switch (Pattern[Pos]) {
                case '^': return BEGIN | EMPTY | LOOK_BEHIND;
                case '$': return END | EMPTY | LOOK_AHEAD;
                case 'b': return BEGIN | END | WORD_BOUNDARY;
                case 'B': return EMPTY | IN_WORD;
            }
            if (exists_in_position("(?=", Pattern, Pos))    return LOOK_AHEAD;
            if (exists_in_position("(?!", Pattern, Pos))    return LOOK_AHEAD | NEGATIVE;
            if (exists_in_position("(?<=", Pattern, Pos))   return LOOK_BEHIND;
            if (exists_in_position("(?<!", Pattern, Pos))   return LOOK_BEHIND | NEGATIVE;
            return 0;
        }();

        static_assert(check_flag != 0, "Invalied template argment [Pattern, Pos]. Specified not assertion.");

    private:
        template <char C>
        static consteval auto extract_assertion() { return char_to_cstr(C); }

        template <char C>
        requires (C == '(')
        static consteval auto extract_assertion()
        {
            return regex_bracket_inner<Pattern, Pos>::value_with_bracket;
        }

    public:
        static constexpr auto assertion_pattern = extract_assertion<Pattern[Pos]>();
        static constexpr auto end_pos = Pos + assertion_pattern.size();

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        template <std::size_t N>
        static constexpr std::size_t generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            if (s.empty()) return (check_flag & EMPTY)
                ? 0
                : std::string_view::npos;
            if ((check_flag & BEGIN) && offset == 0) return 0;
            if ((check_flag & END) && (s.front() == '\r' || s.front() == '\n') && offset == 0) return 0;
            
            for (; offset < s.size(); offset++) {
                if (offset > 0) {
                    const auto before = s[offset - 1];
                    const auto current = s[offset];

                    // CR と LFの間の場合は改行位置として認識させない
                    if (before != '\r' || current != '\n') {
                        if ((check_flag & BEGIN) && (before == '\r' || before == '\n')) return offset;
                        if ((check_flag & END) && (current == '\r' || current == '\n')) return offset;
                    }

                    const auto is_before_word = regex_char_class::get_const_char_set<'w'>().contains(char_to_cstr(before));
                    const auto is_current_word = regex_char_class::get_const_char_set<'w'>().contains(char_to_cstr(current));
                    if ((check_flag & WORD_BOUNDARY) && is_before_word != is_current_word) return offset;
                    if ((check_flag & IN_WORD) && is_before_word && is_current_word) return offset;
                }
                if (is_pos_lock) break;
            }

            if ((check_flag & BEGIN) && (s.back() == '\r' || s.back() == '\n') && offset == s.size()) return offset;

            return (check_flag & END) && offset == s.size()
                ? offset
                : std::string_view::npos;
        }
    };

    /**
     * @fn
     * @brief 言明について解析
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_assertion_parser
        : public regex_assertion_boundary_parser<Pattern, Pos>
    {};
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_ASSERTION_HPP
