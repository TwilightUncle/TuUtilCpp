///----------------------------------
/// @file resolve.hpp 
/// @brief regex.hpp�֘A(�e�p�^�[���̕����񂪂ǂ̋@�\��������������)
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESOLVE_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESOLVE_HPP

namespace tuutil::str::_regex
{
    /**
     * @fn
     * @brief �ǂ̋@�\�̉�͂��s���Ă��邩�A�������ꉻ�ŉ���
    */
    template <cstr Pattern, std::size_t Pos>
    struct resolve_parser : public general<Pattern, Pos> {};

    namespace sp
    {
        /**
         * @fn
         * @brief �����N���X�̏ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        // requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
        struct resolve_parser : public char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �����W���̏ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
        struct resolve_parser<Pattern, Pos> : public char_set_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief ����($��^)�̏ꍇ
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::ANCHOR))
        struct resolve_parser<Pattern, Pos> : public assertion_parser<Pattern, Pos> {};
    }

    /**
     * @fn
     * @brief �ʏ�̓��ꕶ��
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]])
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
        && Pattern[Pos] != '\\'
        && Pattern[Pos] != '('
    )
    struct resolve_parser<Pattern, Pos> : public sp::resolve_parser<Pattern, Pos> {};

    namespace br
    {
        /**
         * @fn
         * @brief ���ʂɑ��������ɑ΂��ĉ�������(�f�t�H���g�̓L���v�`��)
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_parser : public capture_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �O��ǂ݂̌���
        */
        template <cstr Pattern, std::size_t Pos>
        requires (
            exists_in_position("(?=", Pattern, Pos)
            || exists_in_position("(?!", Pattern, Pos)
            || exists_in_position("(?<=", Pattern, Pos)
            || exists_in_position("(?<!", Pattern, Pos)
        )
        struct resolve_parser<Pattern, Pos> : public assertion_parser<Pattern, Pos> {};
    }

    /**
     * @fn
     * @brief �ۊ��ʂ���n�܂�ꍇ
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '(')
    struct resolve_parser<Pattern, Pos> : public br::resolve_parser<Pattern, Pos> {};

    namespace bk
    {
        /**
         * @fn
         * @brief �o�b�N�X���b�V���ɑ��������ɑ΂��ĉ�������(�f�t�H���g�̓G�X�P�[�v)
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_parser : public general<Pattern, Pos> {};

        /**
         * @fn
         * @brief �L���v�`���Q��
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::REFERENCE))
        struct resolve_parser<Pattern, Pos> : public reference_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �o�b�N�X���b�V������n�܂镶���N���X
        */
        template <cstr Pattern, std::size_t Pos>
        requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos]))
        struct resolve_parser<Pattern, Pos> : public char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief �o�b�N�X���b�V������n�܂錾��
        */
        template <cstr Pattern, std::size_t Pos>
        requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::ANCHOR, regex_char_attribute::BK>(Pattern[Pos]))
        struct resolve_parser<Pattern, Pos> : public assertion_parser<Pattern, Pos> {};
    }

    /**
     * @fn
     * @brief �o�b�N�X���b�V������n�܂�ꍇ�̋N�_(�����Ńo�b�N�X���b�V���𗘗p����]���͏I���̂ŁA���̕����ɐi��)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos)
    struct resolve_parser<Pattern, Pos> : public bk::resolve_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_RESOLVE_HPP
