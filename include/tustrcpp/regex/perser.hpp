///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp関連(パターン解析機能を一つのクラスに集積したもの)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP

namespace tustr
{
    /**
     * @fn
     * @brief どの機能の解析を行っているか、部分特殊化で条件分岐。デフォルト
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public add_quantifier<Pattern, regex_general<Pattern, Pos>>
    {
        // 単体で指定してはいけない文字か検証
        using type = add_quantifier<Pattern, regex_general<Pattern, Pos>>;
        static_assert(
            type::end_pos == Pattern.size()
            || !(regex_char_attribute::attributes[Pattern[type::end_pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );
    };

    /**
     * @fn
     * @brief 文字集合の場合
    */
    template <cstr Pattern, std::size_t Pos>
    requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_set_parser<Pattern, Pos>> {};

    /**
     * @fn
     * @brief 文字クラスの場合
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS)
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_class_parser<Pattern, Pos>> {};

    /**
     * @fn
     * @brief 文字クラスの場合2(バックスラッシュ含めて2文字のため、位置を一つ進めている)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos + 1])
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_char_class_parser<Pattern, Pos + 1>> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief バックスラッシュにより、特殊文字としての機能を失った(エスケープ済み)もの(バックスラッシュ含めて2文字のため、位置を一つ進めている)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && bool(regex_char_attribute::attributes[Pattern[Pos + 1]])
        && !bool(regex_char_attribute::attributes[Pattern[Pos + 1]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_general<Pattern, Pos + 1>> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief キャプチャグループ用
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '(')
    struct regex_parser<Pattern, Pos> : public add_quantifier<Pattern, regex_capture_parser<Pattern, Pos>> {};
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP