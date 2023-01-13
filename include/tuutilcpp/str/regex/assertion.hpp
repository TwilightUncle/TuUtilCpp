///----------------------------------
/// @file assertion.hpp 
/// @brief regex.hpp�֘A
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_ASSERTION_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_ASSERTION_HPP

namespace tuutil::str::_regex
{
    /**
     * @fn
     * @brief �����ɂ��ĉ��
    */
    template <cstr Pattern, std::size_t Pos>
    struct assertion_parser
    {
        static constexpr auto begin_pos = Pos;

        enum check_flags
        {
            BEGIN           = 0x0001u,  // �Ώە�����̐擪�Ƀ}�b�`
            END             = 0x0002u,  // �Ώە�����̖����Ƀ}�b�`
            EMPTY           = 0x0004u,  // �󕶎���ɑ΂��ă}�b�`
            WORD_BOUNDARY   = 0x0008u,  // �P��̋��E
            IN_WORD         = 0x0010u,  // �P���
            LOOK_AHEAD      = 0x0020u,  // ��ǂ�
            LOOK_BEHIND     = 0x0040u,  // ��ǂ�
            NEGATIVE        = 0x0080u,  // �ے�I�̏ꍇ���Ă�
        };

        static constexpr auto check_flag = []() -> std::size_t {
            switch (Pattern[Pos]) {
                case '^': return BEGIN | EMPTY | LOOK_BEHIND;
                case '$': return END | EMPTY | LOOK_AHEAD;
                case 'b': return BEGIN | END | WORD_BOUNDARY | LOOK_AHEAD | LOOK_BEHIND;
                case 'B': return EMPTY | IN_WORD | LOOK_AHEAD | LOOK_BEHIND;
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
            return bracket_inner<Pattern, Pos>::value_with_bracket;
        }

    public:
        static constexpr auto assertion_pattern = extract_assertion<Pattern[Pos]>();
        static constexpr auto end_pos = Pos + assertion_pattern.size();

        // �ċA�I�Ȑ��K�\���p�^�[����͂ɓn��������̐���
        static constexpr auto inner_match_pattern = []() {
            if constexpr (assertion_pattern[0] == '(') {
                if constexpr (exists_in_position("(?=", Pattern, Pos) || exists_in_position("(?!", Pattern, Pos))
                    return assertion_pattern.remove_prefix_suffix<3, 1>();
                else return assertion_pattern.remove_prefix_suffix<4, 1>();
            }
            else if constexpr (assertion_pattern[0] == 'b' || assertion_pattern[0] == 'B') return cstr{"\\w"};
            else if constexpr (assertion_pattern[0] == '^' || assertion_pattern[0] == '$') return cstr{"\\r|\\n|\\r\\n"};
            else return cstr{""};
        }();

        static_assert(inner_match_pattern.size() > 0, "Specified assertion match pattern is empty.");

        /**
         * @fn
         * @brief ��͌��ʐ������ꂽ����
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            // �C�Ӄp�^�[���̐�ǂ݁A��ǂ݋��ɑO��ɉ�������̕����񂪑��݂��邱�Ƃ��O��Ǝv���邽�߁A
            // �󕶎��̏ꍇ�͑S�ď�����Ԃ�
            if (s.empty()) return (check_flag & EMPTY)
                ? regex_match_result{0, 0}
                : regex_match_result::make_unmatch();
            if ((check_flag & BEGIN) && offset == 0) return regex_match_result{0, 0};
            
            {
                const auto get_range = [s](std::size_t ofs) {
                    using assertion_matcher = regex<inner_match_pattern, regex_parser>;
                    return assertion_matcher::exec(s, ofs, true).second;
                };
                const auto check_behind = [&get_range](std::size_t ofs) {
                    std::size_t result_pos = ofs + 1;
                    for (
                        std::size_t i = ofs;
                        i > 0 && result_pos != ofs;
                        result_pos = get_range(--i).get_end_pos()
                    );
                    return result_pos == ofs;
                };

                for (; offset <= s.size(); offset++) {
                    bool match = false;
                    bool behind_match = false;
                    bool ahead_match = false;
                    if constexpr (check_flag & LOOK_BEHIND) 
                        match = behind_match = check_behind(offset);
                    if constexpr (check_flag & LOOK_AHEAD)
                        match = ahead_match = (get_range(offset).get_begin_pos() != std::string_view::npos);

                    if constexpr (check_flag & IN_WORD) {
                        if (behind_match && ahead_match) return regex_match_result{offset, 0};
                    }
                    else if constexpr (check_flag & WORD_BOUNDARY) {
                        if (behind_match != ahead_match) return regex_match_result{offset, 0};
                    }
                    else if (bool(check_flag & NEGATIVE) != match) return regex_match_result{offset, 0};

                    if (is_pos_lock) break;
                    // �����T�C�Y�ƈʒu���������ꍇ�A�ʒu���C���N�������g�����Ȃ�
                    if (offset == s.size()) break;
                }
            }

            return (check_flag & END) && offset == s.size()
                ? regex_match_result{offset, 0}
                : regex_match_result::make_unmatch();
        }
    };

    template <class T> struct is_assertion_parser : public std::false_type {};
    template <template <cstr, std::size_t> class Parser, cstr Pattern, std::size_t Pos>
    struct is_assertion_parser<Parser<Pattern, Pos>> : public std::is_base_of<assertion_parser<Pattern, Pos>, Parser<Pattern, Pos>> {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_ASSERTION_HPP
