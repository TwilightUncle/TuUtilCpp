///----------------------------------
/// @file char_set.hpp 
/// @brief regex.hpp�֘A
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CHAR_SET_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CHAR_SET_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief �����W������͂���
    */
    template <cstr Pattern, std::size_t Pos>
    struct char_set_parser
    {
        using brancket_inner = bracket_inner<Pattern, Pos>;
        static constexpr auto char_range = brancket_inner::value;

        static_assert(char_range.size() > 0 && is_collect_regex_back_slash(char_range.view()));

        static constexpr auto allow_or_deny = char_range[0] != '^';
        static constexpr auto begin_index = int(!allow_or_deny);
        static constexpr auto size = char_range.size();
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = brancket_inner::end_pos;

    private:
        /**
         * @fn
         * @brief �����W�����\�z����
        */
        template <int N>
        static constexpr auto make_regex_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else return char_range.substr<N, 1>() + make_regex_char_list<N + 1>();
        }
        template <int N>
        requires (char_range[N] == '-')
        static constexpr auto make_regex_char_list()
        {
            if constexpr (begin_index == N || size - 1 == N)
                return '-' + make_regex_char_list<N + 1>();
            else {
                constexpr char begin_char = (std::min)(char_range[N - 1], char_range[N + 1]) + 1;
                constexpr char end_char = (std::max)(char_range[N - 1], char_range[N + 1]);
                cstr<2 + end_char - begin_char> str{};
                for (char c = 0; c < str.max_size; c++) str[c] = c + begin_char;
                return str + make_regex_char_list<N + 2>();
            }
        }
        template <int N>
        requires (char_range[N] == '\\')
        static constexpr auto make_regex_char_list()
        {
            // �����͈͂̊��ʓ��ł́u.�v�͓��ꕶ���ł͂Ȃ�
            if constexpr (char_range[N + 1] != '.')
                return char_class::get_const_char_set<char_range[N + 1]>() + make_regex_char_list<N + 2>();
            else return '.' + make_regex_char_list<N + 2>();
        }

        /**
         * @fn
         * @brief �o�b�N�X���b�V���ƃZ�b�g�̏ꍇ�̕����W�����\�z����
        */
        template <int N>
        static constexpr auto make_regex_bk_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else if constexpr (char_range[N] == '\\' && char_range[N + 1] != '.')
                return char_class::get_const_bk_char_set<char_range[N + 1]>() + make_regex_bk_char_list<N + 2>();
            else return make_regex_bk_char_list<N + 1>();
        }

    public:
        // �����W��
        static constexpr auto value = make_regex_char_list<begin_index>();

        // �G�X�P�[�v���ꂽ�����ɍ��v���镶���W��
        static constexpr auto bk_value = make_regex_bk_char_list<begin_index>();

        /**
         * @fn
         * @brief ��͌��ʐ������ꂽ����
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            bool is_match = false;
            for (; offset < s.size(); offset++) {
                is_match = value.contains(char_to_cstr(s[offset]));
                if (!allow_or_deny) is_match = !is_match;
                // �ʒu�Œ肳��Ă����ꍇ�A�ŏ��̈ʒu�̂݌��؂��āA���[�v�E�o
                if (is_pos_lock || is_match) break;
            }

            return is_match
                ? regex_match_result{offset, 1}
                : regex_match_result::make_unmatch();
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_CHAR_SET_HPP
