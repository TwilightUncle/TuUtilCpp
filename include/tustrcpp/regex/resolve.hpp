///----------------------------------
/// @file resolve.hpp 
/// @brief regex.hpp関連(各パターンの文字列がどの機能をもつか解決する)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_RESOLVE_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_RESOLVE_HPP

namespace tustr::_regex
{
    /**
     * @fn
     * @brief どの機能の解析を行っているか、部分特殊化で解決
    */
    template <cstr Pattern, std::size_t Pos>
    struct resolve_parser : public general<Pattern, Pos> {};

    namespace sp
    {
        /**
         * @fn
         * @brief 文字クラスの場合
        */
        template <cstr Pattern, std::size_t Pos>
        // requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
        struct resolve_parser : public char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief 文字集合の場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
        struct resolve_parser<Pattern, Pos> : public char_set_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief 言明($と^)の場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::ANCHOR))
        struct resolve_parser<Pattern, Pos> : public assertion_parser<Pattern, Pos> {};
    }

    /**
     * @fn
     * @brief 通常の特殊文字
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
         * @brief 括弧に続く文字に対して解決する(デフォルトはキャプチャ)
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_parser : public capture_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief 前後読みの言明
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
     * @brief 丸括弧から始まる場合
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '(')
    struct resolve_parser<Pattern, Pos> : public br::resolve_parser<Pattern, Pos> {};

    namespace bk
    {
        /**
         * @fn
         * @brief バックスラッシュに続く文字に対して解決する(デフォルトはエスケープ)
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_parser : public general<Pattern, Pos> {};

        /**
         * @fn
         * @brief キャプチャ参照
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::REFERENCE))
        struct resolve_parser<Pattern, Pos> : public reference_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief バックスラッシュから始まる文字クラス
        */
        template <cstr Pattern, std::size_t Pos>
        requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos]))
        struct resolve_parser<Pattern, Pos> : public char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief バックスラッシュから始まる言明
        */
        template <cstr Pattern, std::size_t Pos>
        requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::ANCHOR, regex_char_attribute::BK>(Pattern[Pos]))
        struct resolve_parser<Pattern, Pos> : public assertion_parser<Pattern, Pos> {};
    }

    /**
     * @fn
     * @brief バックスラッシュから始まる場合の起点(ここでバックスラッシュを利用する評価は終わるので、次の文字に進む)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos)
    struct resolve_parser<Pattern, Pos> : public bk::resolve_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief 先頭の正規表現パターンを適用済みとする
    */
    template <template <cstr, RegexParseable> class F, cstr Pattern>
    struct bind_regex_pattern
    {
        template <RegexParseable T>
        struct apply : public F<Pattern, T> {};
        using type = tudb::quote<apply>;
    };

    template <template <cstr, RegexParseable> class F, cstr Pattern> using bind_regex_pattern_t = bind_regex_pattern<F, Pattern>::type;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_RESOLVE_HPP
