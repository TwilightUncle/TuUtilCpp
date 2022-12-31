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
     * @brief 言明について解析
     * キャプチャと同等に再帰的にパターン解析し、境界アサーションは文字列マッチングに置き換えて実行する方向で考える
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_assertion_parser
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

        // 再帰的な正規表現パターン解析に渡す文字列の生成
        static constexpr auto inner_match_pattern = []() {
            if constexpr (assertion_pattern[0] == '(') {
                if constexpr (exists_in_position("(?=", Pattern, Pos) || exists_in_position("(?!", Pattern, Pos))
                    return assertion_pattern.remove_prefix_suffix<3, 1>();
                else return assertion_pattern.remove_prefix_suffix<4, 1>();
            }
            // TODO: 仮。orを定義したら正規表現パターンを返すようにする
            else return cstr{""};
        }();

        // static_assert(inner_match_pattern.size() > 0, "Specified assertion match pattern is empty.");

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        template <std::size_t N>
        static constexpr std::size_t generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            // 任意パターンの先読み、後読み共に前後に何かしらの文字列が存在することが前提と思われるため、
            // 空文字の場合は全て処理を返す
            if (s.empty()) return (check_flag & EMPTY)
                ? 0
                : std::string_view::npos;
            if ((check_flag & BEGIN) && offset == 0) return 0;
            if ((check_flag & END) && (s.front() == '\r' || s.front() == '\n') && offset == 0) return 0;
            
            // TODO: 仮。orを定義したら全て正規表現パターンで判定を行うよう修正
            if (inner_match_pattern.empty())
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
            else {
                const auto get_range = [s](std::size_t ofs) {
                    using assertion_matcher = regex<inner_match_pattern, regex_parser>;
                    return assertion_matcher::run(s, ofs, true).second;
                };
                const auto check_behind = [&get_range](std::size_t ofs) {
                    std::size_t result_pos = ofs + 1;
                    for (
                        std::size_t i = ofs;
                        i > 0 && result_pos != ofs;
                        result_pos = get_range(--i).get_end_pos()
                    );
                    return (result_pos == ofs) ? ofs : std::string_view::npos;
                };

                for (; offset <= s.size(); offset++) {
                    auto pos = std::string_view::npos;
                    if constexpr (check_flag & LOOK_BEHIND) 
                        pos = check_behind(offset);
                    if constexpr (check_flag & LOOK_AHEAD)
                        pos = get_range(offset).get_begin_pos();
                    if (bool(check_flag & NEGATIVE) == (pos == std::string_view::npos)) return offset;
                    if (is_pos_lock) break;
                }
            }

            if ((check_flag & BEGIN) && (s.back() == '\r' || s.back() == '\n') && offset == s.size()) return offset;
            return (check_flag & END) && offset == s.size()
                ? offset
                : std::string_view::npos;
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_ASSERTION_HPP
