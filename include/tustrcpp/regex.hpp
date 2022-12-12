///----------------------------------
/// @file regex.hpp 
/// @brief 定数式に置ける正規表現(ユニコード未対応)
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

namespace tustr
{
    /**
     * @fn
     * @brief 渡された文字範囲を許可/拒否リストに展開し、与えられた文字列がマッチするか判定する関数オブジェクトを返却する
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
     * @class
     * @brief 正規表現格納オブジェクト。動的に生成されたパターンについては考慮しない
    */
    template <cstr Pattern>
    struct regex
    {
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        using flag_type = std::regex_constants::syntax_option_type;

        /**
         * @fn
         * @brief 括弧内の正規表現検証
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_bracket_inner(const std::string_view& bracket_inner, const char bracket_be)
        {
            switch (bracket_be) {
                case '[':
                    // 再帰的な括弧は存在しないため、拒否リストに開始括弧が含まれていたらアウト
                    if (!is_allowed_string(std::string(bracket_inner), false, "[", "")) return std::regex_constants::error_brack;
                    break;
                case '(':
                    // キャプチャリストは再帰的に
                    if (const auto error = parse_pattern(bracket_inner)) return error;
                    break;
                case '{':
                    // 数量詞とユニコードの指定のみのため、許可リストで確認
                    if (!is_allowed_string(std::string(bracket_inner), true, ",0123456789abcdef", "")) return std::regex_constants::error_brack;
                    break;
            }
            return std::nullopt;
        }

        /**
         * @fn
         * @brief 正規表現の文法の検証
        */
        static constexpr std::optional<std::regex_constants::error_type> parse_pattern(const std::string_view& pattern_view)
        {
            // バックスラッシュの検証
            if (is_collect_regex_back_slash(pattern_view)) return std::regex_constants::error_escape;

            // 一文字ずつ検証
            for (
                auto [i, ch] = std::pair{0, pattern_view[0]};
                i < pattern_view.size();
                ch = pattern_view[++i]
            ) {
                const auto attr = get_regex_character_attribute(ch);
                // 特殊文字ではない
                if (!(attr & REGEX_SPECIAL_MASK)) continue;

                // 特殊文字の前にバックスラッシュありかの判定
                const auto is_before_bk = eq_before_char(pattern_view, i, '//');
                if ((attr & (REGEX_BK_SPECIAL | REGEX_CLASSES_BK_SPECIAL) && !is_before_bk)) continue;
                if ((attr & (REGEX_SPECIAL | REGEX_CLASSES_SPECIAL)) && is_before_bk) continue;

                // とじ括弧がいきなり現れるのはおかしいためエラー
                if (attr & REGEX_BRANCKET_EN) {
                    switch (ch) {
                        case ']': return std::regex_constants::error_brack;
                        case ')': return std::regex_constants::error_paren;
                        case '}': return std::regex_constants::error_brace;
                    }
                }

                // 括弧を検出
                // if (attr & REGEX_BRANCKET_BE) {
                //     const auto [inner, en_pos, br_error] = extract_regex_bracket_inner(pattern_view, i);
                //     if (br_error) return br_error;
                //     // 括弧内部の検証は別関数に委譲のため、とじ括弧までインデックスを進める
                //     if (const auto inner_error = parse_bracket_inner(inner, ch)) return inner_error;
                //     i = en_pos;
                //     continue;
                // }
            }
            return std::nullopt;
        }

        // consteval regex()
        // {
        //     // 正規表現のフラグ,文法チェック
        //     // assert(validate_syntax_option(syntax));
        //     static_assert(is_collect_regex_back_slash(Pattern.view()));
        //     // assert(validate_regex_syntax(pattern.view(), syntax));
        // };
        
        /**
         * @fn
         * @brief syntaxを指定するビットのみ抜き出す
        */
        static constexpr flag_type get_syntax_bits(flag_type syntax)
        {
            constexpr auto syntax_mask = std::regex_constants::ECMAScript
                | std::regex_constants::basic
                | std::regex_constants::extended
                | std::regex_constants::awk
                | std::regex_constants::grep
                | std::regex_constants::egrep;
            return syntax & syntax_mask;
        }

        /**
         * @fn
         * @brief syntaxに一つも指定されていない場合、ECMAScriptのビットを立てる
        */
        static constexpr flag_type shap_syntax_option(flag_type syntax)
        {
            if (!get_syntax_bits(syntax))
                syntax |= flag_type::ECMAScript;
            return syntax;
        }
    
        /**
         * @fn
         * @brief syntaxの検証
        */
        static constexpr bool validate_syntax_option(flag_type syntax)
        {
            // return std::popcount((std::uint64_t)get_syntax_bits(syntax)) <= 1;
            // 現在ECMAScriptバージョンしか実装していないので、ほかの文法が指定されていたらNGとする
            return get_syntax_bits(shap_syntax_option(syntax)) == flag_type::ECMAScript;
        }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
