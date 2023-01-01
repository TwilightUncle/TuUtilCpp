///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp関連(パターン解析機能を一つのクラスに集積したもの)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP

namespace tustr
{
    namespace _regex
    {
        /**
         * @fn
         * @brief どの機能の解析を行っているか、部分特殊化で名前解決
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

        /**
         * @fn
         * @brief 文字集合の場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
        struct resolve_regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief 文字クラスの場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
        struct resolve_regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

        /**
         * @fn
         * @brief 言明($と^)の場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::ANCHOR) && Pattern[Pos] != '(')
        struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};

        namespace br
        {
            /**
             * @fn
             * @brief 括弧に続く文字に対して解決する(デフォルトはキャプチャ)
            */
            template <cstr Pattern, std::size_t Pos>
            struct resolve_regex_parser : public regex_capture_parser<Pattern, Pos> {};

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
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
        }

        /**
         * @fn
         * @brief 丸括弧から始まる場合
        */
        template <cstr Pattern, std::size_t Pos>
        requires (Pattern[Pos] == '(')
        struct resolve_regex_parser<Pattern, Pos> : public br::resolve_regex_parser<Pattern, Pos> {};

        namespace bk
        {
            /**
             * @fn
             * @brief バックスラッシュに続く文字に対して解決する(デフォルトはエスケープ)
            */
            template <cstr Pattern, std::size_t Pos>
            struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

            /**
             * @fn
             * @brief キャプチャ参照
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::REFERENCE))
            struct resolve_regex_parser<Pattern, Pos> : public regex_reference_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief バックスラッシュから始まる文字クラス
            */
            template <cstr Pattern, std::size_t Pos>
            requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos]))
            struct resolve_regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief バックスラッシュから始まる言明
            */
            template <cstr Pattern, std::size_t Pos>
            requires (regex_char_attribute::check_attrs_conjuction<regex_char_attribute::ANCHOR, regex_char_attribute::BK>(Pattern[Pos]))
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
        }

        /**
         * @fn
         * @brief バックスラッシュから始まる場合の起点(ここでバックスラッシュを利用する評価は終わるので、次の文字に進む)
        */
        template <cstr Pattern, std::size_t Pos>
        requires (Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos)
        struct resolve_regex_parser<Pattern, Pos> : public bk::resolve_regex_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

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

    
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public regex_or_parser<Pattern, get_or_pos_regex_pattern(Pattern)> {};

    /**
     * @class
     * @brief 正規表現パターンの指定個所の解析を行う(継承は一括で適用したい関数が増えることを想定して、右畳み込みで実装)
     * @tparam Pattern 正規表現パターンの文字列リテラルを指定
     * @tparam Pos Patternの解析する開始位置を指定
    */
    template <cstr Pattern, std::size_t Pos>
    requires (get_or_pos_regex_pattern(Pattern) == std::string_view::npos)
    struct regex_parser<Pattern, Pos> : public tudb::foldr_t<
        tudb::quote<tudb::apply>,
        _regex::bind_regex_pattern_t<add_quantifier, Pattern>,
        _regex::resolve_regex_parser<Pattern, Pos>
    > {
        // 単体で指定してはいけない文字か検証
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
