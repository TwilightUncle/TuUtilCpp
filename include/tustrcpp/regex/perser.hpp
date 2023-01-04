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
         * @brief どの機能の解析を行っているか、部分特殊化で解決
        */
        template <cstr Pattern, std::size_t Pos>
        struct resolve_regex_parser : public regex_general<Pattern, Pos> {};

        namespace sp
        {
            /**
             * @fn
             * @brief 文字クラスの場合
            */
            template <cstr Pattern, std::size_t Pos>
            // requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS))
            struct resolve_regex_parser : public regex_char_class_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief 文字集合の場合
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
            struct resolve_regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

            /**
             * @fn
             * @brief 言明($と^)の場合
            */
            template <cstr Pattern, std::size_t Pos>
            requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::ANCHOR))
            struct resolve_regex_parser<Pattern, Pos> : public regex_assertion_parser<Pattern, Pos> {};
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
        struct resolve_regex_parser<Pattern, Pos> : public sp::resolve_regex_parser<Pattern, Pos> {};

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

    /**
     * @class
     * @brief 正規表現パターン解析(orの場合)
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser
    {
        using parsed_type = regex_or_parser<Pattern, get_or_pos_regex_pattern(Pattern)>;
        static constexpr std::size_t max_capture_count = parsed_type::inner_regex::parser::max_capture_count;

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            return parsed_type::template generated_func<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);
        }
    };

    /**
     * @class
     * @brief 正規表現パターンの指定個所の解析を行う(継承は一括で適用したい関数が増えることを想定して、右畳み込みで実装)
     * @tparam Pattern 正規表現パターンの文字列リテラルを指定
     * @tparam Pos Patternの解析する開始位置を指定
    */
    template <cstr Pattern, std::size_t Pos>
    requires (get_or_pos_regex_pattern(Pattern) == std::string_view::npos && Pattern.size() > Pos)
    struct regex_parser<Pattern, Pos>
    {
        // 単体で指定してはいけない文字か検証
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

        using parsed_type = tudb::foldr_t<
            tudb::quote<tudb::apply>,
            _regex::bind_regex_pattern_t<add_quantifier, Pattern>,
            _regex::resolve_regex_parser<Pattern, Pos>
        >;

        using parsed_next_type = regex_parser<Pattern, parsed_type::end_pos>;

        static constexpr std::size_t max_capture_count = []() {
            constexpr std::size_t allowed_max_capture_count = 65535;
            std::size_t cnt{};
            // 再帰的にregexが定義されている場合
            if constexpr (InnerRegexReferable<parsed_type>)
                cnt += (parsed_type::max_count == std::string_view::npos)
                    ? allowed_max_capture_count
                    : parsed_type::inner_regex::parser::max_capture_count * parsed_type::max_count;
            // 自身がキャプチャ
            if constexpr (RegexParserCaptureable<parsed_type>)
                cnt += (std::min)(allowed_max_capture_count, parsed_type::max_count);
            return (std::min)(allowed_max_capture_count, cnt + parsed_next_type::max_capture_count);
        }();

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            std::size_t cnt = 0;
            regex_capture_store<MaxCaptureCount> temp_capture_store = capture_store;
            auto result = regex_match_result::make_unmatch();
            bool is_lock = is_pos_lock;

            for (auto end_pos = offset, begin_pos = std::string_view::npos; end_pos < subject.size() && cnt < parsed_type::max_count;) {
                // 判定が真の時のみ値が更新されるよう、一旦一時変数に更新値を記録
                auto cs = temp_capture_store;
                const auto temp_result = parsed_type::template generated_func2<MaxCaptureCount>(
                    subject,
                    end_pos,
                    std::exchange(is_lock, true),
                    cs
                );
                if (!temp_result) break;

                // 判定通ったので更新
                begin_pos = (std::min)(begin_pos, temp_result.get_begin_pos());
                end_pos = temp_result.get_end_pos();
                temp_capture_store = cs;

                if (++cnt >= parsed_type::min_count) {
                    const auto re = parsed_next_type::exec<MaxCaptureCount>(subject, end_pos, true, temp_capture_store);
                    if (!re) continue;
                    result.set_begin_pos(begin_pos);
                    result.set_end_pos(re);
                    capture_store = temp_capture_store;
                    if constexpr (parsed_type::negative) break;
                }
            }

            // 0回マッチでもOKの場合
            if (!cnt && !parsed_type::min_count)
                result = parsed_next_type::exec<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);

            return (cnt < parsed_type::min_count)
                    ? regex_match_result::make_unmatch()
                    : result;
        }
    };

    /**
     * @class
     * @brief 再帰定義の終点
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern.size() <= Pos)
    struct regex_parser<Pattern, Pos>
    {
        static constexpr std::size_t max_capture_count = 0;

        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>&)
        {
            return regex_match_result{offset, 0};
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
