///----------------------------------
/// @file capture.hpp 
/// @brief regex.hpp�֘A
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CAPUTURE_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CAPUTURE_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief �L���v�`���O���[�v�̉��
    */
    template <cstr Pattern, std::size_t Pos>
    struct capture_parser
    {
        using brancket_inner = bracket_inner<Pattern, Pos>;
        static constexpr auto inner_pattern = brancket_inner::value;

        static_assert(inner_pattern.size() > 0 && is_collect_regex_back_slash(inner_pattern.view()));

        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = brancket_inner::end_pos;

        static constexpr auto is_capture = !exists_in_position("?:", inner_pattern, 0);

    private:
        /**
         * @fn
         * @brief �L���v�`���̖��̂��擾
        */
        template <char C> requires (C == '?' && exists_in_position("?<", inner_pattern, 0))
        static constexpr auto extract_capture_name()
        {
            using named_bracket = bracket_inner<inner_pattern, 1>;
            constexpr auto named_inner = named_bracket::value;

            static_assert(
                named_inner.size() > 0 && named_inner.match_all_charset(char_class::get_const_char_set<'w'>()),
                "An error has occurred. [capture_parser]"
            );

            return named_inner;
        }
        template <char C>
        static constexpr auto extract_capture_name() { return cstr{""}; }

    public:
        static constexpr auto name = extract_capture_name<inner_pattern[0]>();
        static constexpr auto is_named = name.size() > 0;

        // �L���v�`���̗L���A�L���v�`�����w�蕔������菜�����p�^�[��
        static constexpr auto capture_pattern = inner_pattern.template remove_prefix<(is_capture ? 0 : 2) + (is_named ? name.size() + 3 : 0)>();

        using inner_regex = regex<capture_pattern, regex_parser>;

        /**
         * @fn
         * @brief ��͌��ʐ������ꂽ����
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto [cap, re] = inner_regex::exec(s, offset, is_pos_lock);
            if (re) {
                if (is_capture) cs.push_back(s.substr(offset, re.match_length()), name.view());
                cs.push_back(cap);
            } 
            return re;
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CAPUTURE_HPP
