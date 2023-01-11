///----------------------------------
/// @file regex.hpp 
/// @brief 定数式に置ける正規表現(ユニコード未対応)
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_HPP

#include <regex>
#include <tustrcpp/cstr.hpp>
#include <tustrcpp/regex/result.hpp>
#include <tuutilcpp/mpl.hpp>

namespace tustr
{
    /**
     * @brief forやwhileで回せるように、関数へ変換する際の型
    */
    template <std::size_t N>
    using regex_generated_function_ptr_type = regex_match_result(*)(std::string_view, std::size_t, bool, regex_capture_store<N>&);

    template <class T>
    concept RegexParseable = requires {
        { T::begin_pos } -> std::convertible_to<std::size_t>;
        { T::end_pos } -> std::convertible_to<std::size_t>;

        {
            T::template generated_func<0>(
                std::declval<std::string_view>(),
                std::declval<std::size_t>(),
                std::declval<bool>(),
                std::declval<regex_capture_store<0>&>()
            )
        } -> std::same_as<regex_match_result>;
        {
            T::template generated_func<1>(
                std::declval<std::string_view>(),
                std::declval<std::size_t>(),
                std::declval<bool>(),
                std::declval<regex_capture_store<1>&>()
            )
        } -> std::same_as<regex_match_result>;
        {
            T::template generated_func<10>(
                std::declval<std::string_view>(),
                std::declval<std::size_t>(),
                std::declval<bool>(),
                std::declval<regex_capture_store<10>&>()
            )
        } -> std::same_as<regex_match_result>;

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
        { T::is_capture } -> std::convertible_to<bool>;
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
#include <tustrcpp/regex/reference.hpp>
#include <tustrcpp/regex/or.hpp>
#include <tustrcpp/regex/assertion.hpp>
#include <tustrcpp/regex/perser.hpp>

namespace tustr
{
    /**
     * @class
     * @brief 正規表現格納オブジェクト。動的に生成されたパターンについては考慮しない。インスタンスにはパターンマッチの結果が格納される。
     * @tparam Pattern 正規表現の文字列リテラルを指定
     * @tparam Parser 正規表現を解析するテンプレートクラスを指定(option)
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
        // 文法上おかしいバックスラッシュの出現はここで検出、エラーとする
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        using parser = Parser<Pattern, 0>;
        using capture_store_type = regex_capture_store<parser::max_capture_count>;
        using function_ptr_type = regex_generated_function_ptr_type<parser::max_capture_count>;

        static constexpr auto exec(std::string_view s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            auto cs = regex_capture_store<parser::max_capture_count>{};
            auto re = parser::exec<parser::max_capture_count>(s, offset, is_pos_lock, cs);
            return std::pair{cs, re};
        }

    private:
        // 結果キャプチャリストを格納
        const capture_store_type capture_list;
        // 結果のマッチ範囲を格納
        const regex_match_result match_range;
        // テスト対象
        const std::string_view test_target;

        constexpr regex(std::string_view test_target, const std::pair<capture_store_type, regex_match_result>& run_result)
            : capture_list(run_result.first)
            , match_range(run_result.second)
            , test_target(test_target)
        {}

    public:
        /**
         * @fn
         * @brief パターンマッチを行い、インスタンスには結果を格納する
         * @param test_target パターンマッチの対象文字列
         * @param offset 対象文字列のパターンマッチを開始する位置
         * @param is_pos_lock 真の場合パターンマッチの位置をoffsetで固定する。offsetの位置から一致していない場合一致なしとなる
        */
        constexpr regex(std::string_view test_target, std::size_t offset = 0, bool is_pos_lock = false)
            : regex(test_target, exec(test_target, offset, is_pos_lock))
        {}

        /**
         * @fn
         * @brief 検査対象文字列にパターンマッチしている部分が存在したら真
        */
        constexpr bool exists() const noexcept { return bool(match_range); }

        /**
         * @fn
         * @brief 検査対象の文字列全体がパターンマッチしているとき真
        */
        constexpr bool is_match() const noexcept { return match_range.match_length() == test_target.size(); }

        constexpr bool empty() const noexcept { return !this->exists(); }
        constexpr std::size_t size() const noexcept { return this->exists() ? this->capture_list.size() + 1 : 0; }

        /**
         * @fn
         * @brief パターンマッチした部分の文字列を取得する。抽出結果なしの場合空文字を返却
         * @param index 0を指定すると、一致個所全体、1以上の値を設定すると該当するキャプチャのマッチを取得
        */
        constexpr std::string_view get_match_string_view(std::size_t index = 0) const
        {
            if (!this->exists() || this->capture_list.size() < index) return std::string_view{};
            if (!index) return test_target.substr(match_range.get_begin_pos(), match_range.match_length());
            return this->capture_list.get(index - 1);
        }

        constexpr std::string_view operator[](std::size_t n) const { return this->get_match_string_view(n); }

        /**
         * @fn
         * @brief 引数の文字列内に、パターンマッチする部分が存在していれば真
        */
        static constexpr bool search(std::string_view s) { return regex(s).exists(); }

        /**
         * @fn
         * @brief 引数の文字列の全体がマッチしている場合真
        */
        static constexpr bool match(std::string_view s) { return regex(s).is_match(); }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
