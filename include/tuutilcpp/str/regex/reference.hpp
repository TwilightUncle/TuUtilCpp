///----------------------------------
/// @file str/regex/reference.hpp 
/// @brief regex.hpp�֘A(����Q��)
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP

namespace tuutil::str::_regex
{
    /**
     * @brief �C���f�b�N�X�ɂ��L���v�`���̌���Q��
     * @tparam Pattern ���K�\���p�^�[���̌Œ蒷������
     * @tparam Pos ��͊J�n�ʒu
    */
    template <cstr Pattern, std::size_t Pos>
    struct reference_parser
    {
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = Pos + Pattern.get_charset_match_range("0123456789", Pos)[1];
        static constexpr auto reference_index = to_int<std::size_t>(Pattern.template substr<Pos, end_pos - Pos>().view()) - 1;

        /**
         * @fn
         * @brief ���ɃL���v�`�������݂��邱�Ƃ��O��̂��߁Ais_pos_lock�̐^�U�Ɋւ�炸�ʒu�͏�ɌŒ��OK
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto search_str = cs.get(reference_index);
            return exists_in_position(search_str, s, offset)
                ? regex_match_result{offset, search_str.size()}
                : regex_match_result::make_unmatch();
        }
    };

    /**
     * @brief ���O�t���L���v�`���̌���Q��
     * @tparam Pattern ���K�\���p�^�[���̌Œ蒷������
     * @tparam Pos ��͊J�n�ʒu
    */
    template <cstr Pattern, std::size_t Pos>
    requires (exists_in_position("k<", Pattern, Pos))
    struct reference_parser<Pattern, Pos>
    {
        static constexpr auto begin_pos = Pos;
        using named_bracket = bracket_inner<Pattern, Pos + 1>;
        static constexpr auto reference_name = named_bracket::value;
        static constexpr auto end_pos = Pos + 1 + named_bracket::value_with_bracket.size();

        // �L���v�`�����͈ꕶ���ȏォ�A���p�p����_�̂ݎw��\�Ƃ��Ă���ȊO�̓R���p�C���G���[
        static_assert(
            reference_name.size() > 0 && reference_name.match_all_charset(char_class::get_const_char_set<'w'>()),
            "An error has occurred. [named reference_parser]"
        );

        /**
         * @fn
         * @brief ���ɃL���v�`�������݂��邱�Ƃ��O��̂��߁Ais_pos_lock�̐^�U�Ɋւ�炸�ʒu�͏�ɌŒ��OK
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto search_str = cs.get(reference_name.view());
            return exists_in_position(search_str, s, offset)
                ? regex_match_result{offset, search_str.size()}
                : regex_match_result::make_unmatch();
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_REFERENCE_HPP
