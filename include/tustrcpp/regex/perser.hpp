///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp関連(パターン解析機能を一つのクラスに集積したもの)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP

#include <tustrcpp/regex/resolve.hpp>

namespace tustr
{
    /**
     * @class
     * @brief 正規表現パターン解析(orの場合)
     * @tparam Pattern 正規表現パターンの文字列リテラルを指定
     * @tparam Pos Patternの解析する開始位置を指定
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser
    {
        using parsed_type = _regex::or_parser<Pattern, _regex::get_or_pos_regex_pattern(Pattern)>;

        // 静的に領域を確保するため、正規表現パターンからわかる最大キャプチャ数を取得
        static constexpr std::size_t max_capture_count = parsed_type::inner_regex::parser::max_capture_count;

        /**
         * @fn
         * @brief パターンマッチング実行
         * @param subject 検証対象文字列
         * @param offset subjectの検証開始位置
         * @param is_pos_lock offsetの位置から一致判定を実施する場合真。偽を指定した場合offset以降の任意の場所で一致しているか判定する
         * @param capture_store キャプチャ内容の追加と参照を実施する
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            return parsed_type::template generated_func<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);
        }
    };

    /**
     * @class
     * @brief 正規表現パターンの指定個所の解析を行う(or以外の正規表現を解析するための特殊化)
     * @tparam Pattern 正規表現パターンの文字列リテラルを指定
     * @tparam Pos Patternの解析する開始位置を指定
    */
    template <cstr Pattern, std::size_t Pos>
    requires (_regex::get_or_pos_regex_pattern(Pattern) == std::string_view::npos && Pattern.size() > Pos)
    struct regex_parser<Pattern, Pos>
    {
        // 単体で指定してはいけない文字か検証
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

        using parsed_type = tudb::foldr_t<
            tudb::quote<tudb::apply>,
            _regex::bind_regex_pattern_t<_regex::add_quantifier, Pattern>,
            _regex::resolve_parser<Pattern, Pos>
        >;

        // 以降の位置の解析
        using parsed_next_type = regex_parser<Pattern, parsed_type::end_pos>;

        // 静的に領域を確保するため、正規表現パターンからわかる最大キャプチャ数を取得
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

        /**
         * @fn
         * @brief パターンマッチング実行。数量詞のループもここで制御
         * @param subject 検証対象文字列
         * @param offset subjectの検証開始位置
         * @param is_pos_lock offsetの位置から一致判定を実施する場合真。偽を指定した場合offset以降の任意の場所で一致しているか判定する
         * @param capture_store キャプチャ内容の追加と参照を実施する
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            std::size_t cnt = 0;
            regex_capture_store<MaxCaptureCount> temp_capture_store = capture_store;
            auto result = regex_match_result::make_unmatch();
            bool is_lock = is_pos_lock;

            for (auto end_pos = offset, begin_pos = std::string_view::npos; end_pos < subject.size() && cnt < parsed_type::max_count;) {
                const auto temp_result = parsed_type::template generated_func<MaxCaptureCount>(
                    subject,
                    end_pos,
                    std::exchange(is_lock, true),
                    temp_capture_store
                );
                if (!temp_result) break;

                // 判定通ったので更新
                begin_pos = (std::min)(begin_pos, temp_result.get_begin_pos());
                end_pos = temp_result.get_end_pos();

                if (++cnt >= parsed_type::min_count) {
                    // 判定が真の時のみ値が更新されるよう、一旦一時変数に更新値を記録
                    auto cs = temp_capture_store;
                    const auto re = parsed_next_type::exec<MaxCaptureCount>(subject, end_pos, true, cs);
                    if (!re) continue;
                    result.set_begin_pos(begin_pos);
                    result.set_end_pos(re);
                    capture_store = cs;
                    if constexpr (!parsed_type::greedy) break;
                }
            }

            // 0回マッチでもOKの場合
            if (!cnt && !parsed_type::min_count)
                result = parsed_next_type::exec<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);
            // 0

            return (cnt < parsed_type::min_count)
                    ? regex_match_result::make_unmatch()
                    : result;
        }
    };

    /**
     * @class
     * @brief 再帰表現パターン解析(再帰定義の終点の特殊化)
     * @tparam Pattern 正規表現パターンの文字列リテラルを指定
     * @tparam Pos Patternの解析する開始位置を指定
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern.size() <= Pos)
    struct regex_parser<Pattern, Pos>
    {
        // 静的に領域を確保するため、正規表現パターンからわかる最大キャプチャ数を取得
        static constexpr std::size_t max_capture_count = 0;

        /**
         * @fn
         * @brief パターンマッチング実行
         * @param subject 検証対象文字列
         * @param offset subjectの検証開始位置
         * @param is_pos_lock offsetの位置から一致判定を実施する場合真。偽を指定した場合offset以降の任意の場所で一致しているか判定する
         * @param capture_store キャプチャ内容の追加と参照を実施する
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>&)
        {
            return regex_match_result{offset, 0};
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_PERSER_HPP
