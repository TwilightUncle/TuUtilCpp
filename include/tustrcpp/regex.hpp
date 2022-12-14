///----------------------------------
/// @file regex.hpp 
/// @brief �萔���ɒu���鐳�K�\��(���j�R�[�h���Ή�)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>

#include <tustrcpp/regex/common.hpp>
#include <tustrcpp/regex/bracket.hpp>
#include <tustrcpp/regex/char_class.hpp>
#include <tustrcpp/regex/quantifier.hpp>
#include <tustrcpp/regex/char_set.hpp>
#include <tustrcpp/regex/capture.hpp>
#include <tustrcpp/regex/general.hpp>

namespace tustr
{
    /**
     * @fn
     * @brief �n���ꂽ�����͈͂�����/���ۃ��X�g�ɓW�J���A�^����ꂽ�����񂪃}�b�`���邩���肷��֐��I�u�W�F�N�g��ԋp����
    */
    template <cstr Pattern, std::size_t Pos>
    constexpr auto get_regex_char_range_matcher()
    {
        return [](const std::string& comp) -> bool {
            using range_parser = regex_char_set_parser<Pattern, Pos>;
            return is_allowed_string(
                comp,
                range_parser::allow_or_deny,
                range_parser::value.data(),
                range_parser::bk_value.data(),
                false
            );
        };
    }

    /**
     * @fn
     * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�ŏ�������B�f�t�H���g
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public regex_general<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����W���̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
    struct regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS)
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief �����N���X�̏ꍇ2(�o�b�N�X���b�V���܂߂�2�����̂��߁A�ʒu����i�߂Ă���)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\'
        && Pattern.size() - 1 > Pos
        && regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos])
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

    /**
     * @class
     * @brief ���K�\���i�[�I�u�W�F�N�g�B���I�ɐ������ꂽ�p�^�[���ɂ��Ă͍l�����Ȃ�
    */
    template <cstr Pattern>
    struct regex
    {
    private:
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        /**
         * @fn
         * @brief ���K�\���̉�́B���ꕶ�����܂܂Ȃ����̒S��
        */
        template <std::size_t Pos, std::size_t N>
        static consteval auto parse()
        {
            // ���炩���߁APattern[Pos]�̒l�ɂ���ĕ������ꉻ���A
            // �Ăяo����鏈�����I�[�o�[���[�h�ł���悤�ɂ���
            using parser = regex_parser<Pattern, Pos>;
            auto parse_result = parse<parser::end_pos, N + 1>();
            parse_result[N] = parser::generated_func;
            return parse_result;
        }

        /**
         * @fn
         * @brief �ċA�̏I�[
        */
        template <std::size_t Pos, std::size_t N>
        requires (Pattern.size() <= Pos)
        static consteval auto parse()
        {
            // �eperser�ňȉ��֐��|�C���^�Ɋi�[�\�Ȋ֐����`���邱�ƂŁA
            // ��͌��ʂ��֐��|�C���^�̔z��Ƃ��ĕێ��ł���悤�ɂ���
            using generated_function_type = std::size_t(*)(const std::string_view&, std::size_t);
            return std::array<generated_function_type, N>{nullptr};
        }

    public:
        static constexpr auto parse_result = parse<0, 0>();
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP