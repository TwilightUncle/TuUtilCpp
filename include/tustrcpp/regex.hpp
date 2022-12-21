///----------------------------------
/// @file regex.hpp 
/// @brief 定数式に置ける正規表現(ユニコード未対応)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>

namespace tustr
{
    /**
     * @brief forやwhileで回せるように、関数へ変換する際の型
    */
    using regex_generated_function_type = std::size_t(const std::string_view&, std::size_t, bool);
    using regex_generated_function_ptr_type = std::size_t(*)(const std::string_view&, std::size_t, bool);

    template <class T>
    concept RegexParseable = requires {
        // 静的メンバが定義済みか
        T::begin_pos;
        T::end_pos;
        T::generated_func;

        // 上記メンバの型チェック
        requires std::is_same_v<decltype(T::begin_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::end_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::generated_func), regex_generated_function_type>; // 関数

        // 違反している場合、無限再帰が発生してしまう
        requires T::begin_pos < T::end_pos;
    };
    
    // 前方宣言
    template <cstr Pattern, std::size_t Pos> struct regex_parser;
    template <cstr Pattern, template <cstr, std::size_t> class Parser> struct regex;

    template <class T> struct is_regex : public std::false_type {};
    template <cstr Pattern, template <cstr, std::size_t> class Parser>
    struct is_regex<regex<Pattern, Parser>> : public std::true_type {};

    // 再帰的に正規表現クラスを内包しているか
    template <class T>
    concept InnerRegexReferable = requires {
        // キャプチャグループ等括弧内を再帰的に解析している場合
        typename T::inner_regex;
        requires is_regex<typename T::inner_regex>::value;
    };

    // キャプチャを行うものか
    template <class T>
    concept RegexParserCaptureable = requires {
        T::is_capture;
        requires std::is_same_v<decltype(T::is_capture), bool>;
        requires T::is_capture;
    };
}

#include <tustrcpp/regex/common.hpp>
#include <tustrcpp/regex/bracket.hpp>
#include <tustrcpp/regex/char_class.hpp>
#include <tustrcpp/regex/quantifier.hpp>
#include <tustrcpp/regex/char_set.hpp>
#include <tustrcpp/regex/capture.hpp>
#include <tustrcpp/regex/general.hpp>
#include <tustrcpp/regex/perser.hpp>

namespace tustr
{
    /**
     * @fn
     * @brief 渡された文字範囲を許可/拒否リストに展開し、与えられた文字列がマッチするか判定する関数オブジェクトを返却する
     * TODO: いらないのでいつかテストの修正と本関数を削除する
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
     * @tparam Pttern 正規表現の文字列リテラルを指定
     * @tparam Parser 正規表現を解析するテンプレートクラスを指定
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
        // キャプチャ可能な最大数(静的に格納結果のキャプチャ領域を確保したいため、上限を設ける)
        static constexpr std::size_t allowed_max_capture_count = 65535;
    private:
        // 文法上おかしいバックスラッシュの出現はここで検出、エラーとする
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        /**
         * @fn
         * @brief 正規表現の解析。特殊文字を含まない個所の担当
        */
        template <std::size_t Pos, std::size_t N>
        static consteval auto parse()
        {
            // あらかじめ、Pattern[Pos]の値によって部分特殊化し、
            // 呼び出される処理をオーバーロードできるようにする
            using parser = Parser<Pattern, Pos>;

            static_assert(RegexParseable<parser>, "Invaild template argment [Parser]. See concept [RegexParseable].");

            auto [f_arr, capture_cnt] = parse<parser::end_pos, N + 1>();
            f_arr[N] = parser::generated_func;

            // 結果を保存しなければならないキャプチャが存在する場合加算
            if constexpr (InnerRegexReferable<parser>)
                capture_cnt = (std::max)(
                    capture_cnt + parser::inner_regex::max_capture_count * parser::max_count,
                    (std::max)(parser::max_count, capture_cnt)
                );
            if constexpr (RegexParserCaptureable<parser>)
                capture_cnt = (std::max)(
                    capture_cnt + parser::max_count,
                    (std::max)(parser::max_count, capture_cnt)
                );

            return std::pair{f_arr, (std::min)(capture_cnt, allowed_max_capture_count)};
        }

        /**
         * @fn
         * @brief 再帰の終端
        */
        template <std::size_t Pos, std::size_t N>
        requires (Pattern.size() <= Pos)
        static consteval auto parse()
        {
            // 各perserで以下関数ポインタに格納可能な関数を定義することで、
            // 解析結果を関数ポインタの配列として保持できるようにする
            return std::pair{
                std::array<regex_generated_function_ptr_type, N>{nullptr},
                std::size_t{}
            };
        }
        static constexpr auto parse_result = parse<0, 0>();

    public:
        // 正規表現のパターンを解析した結果のルールを関数集合としたもの
        static constexpr auto& match_rules = parse_result.first;

        // 想定される最大キャプチャ数
        static constexpr auto max_capture_count = parse_result.second;

        /**
         * @fn
         * @brief パターンマッチの基本。実行後の結果はPtternのsizeとなるが、一致しなかった場合はstd::string_view::nposが返される
        */
        static constexpr std::size_t run(const std::string_view& s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            for(regex_generated_function_ptr_type before = nullptr; const auto& f : match_rules) {
                if ((offset = f(s, offset, std::exchange(is_pos_lock, true))) == std::string_view::npos) return offset;
                before = f;
            }
            return offset;
        }

        static constexpr bool match(const std::string_view& s) { return run(s, 0) != std::string_view::npos; }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
