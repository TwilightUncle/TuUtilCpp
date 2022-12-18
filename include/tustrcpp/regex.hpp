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
#include <tustrcpp/regex/general.hpp>

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
     * @fn
     * @brief どの機能の解析を行っているか、部分特殊化で条件分岐。デフォルト
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser : public regex_general<Pattern, Pos> {};

    /**
     * @fn
     * @brief 文字集合の場合
    */
    template <cstr Pattern, std::size_t Pos>
    requires (bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CHARSET))
    struct regex_parser<Pattern, Pos> : public regex_char_set_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief 文字クラスの場合
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::CLASS)
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos> {};

    /**
     * @fn
     * @brief 文字クラスの場合2(バックスラッシュ含めて2文字のため、位置を一つ進めている)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && regex_char_attribute::check_attrs_conjuction<regex_char_attribute::CLASS, regex_char_attribute::BK>(Pattern[Pos])
    )
    struct regex_parser<Pattern, Pos> : public regex_char_class_parser<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

    /**
     * @fn
     * @brief バックスラッシュにより、特殊文字としての機能を失った(エスケープ済み)もの(バックスラッシュ含めて2文字のため、位置を一つ進めている)
    */
    template <cstr Pattern, std::size_t Pos>
    requires (
        Pattern[Pos] == '\\' && Pattern.size() - 1 > Pos
        && bool(regex_char_attribute::attributes[Pattern[Pos]])
        && !bool(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::BK)
    )
    struct regex_parser<Pattern, Pos> : public regex_general<Pattern, Pos + 1> { static constexpr auto begin_pos = Pos; };

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
        requires std::is_same_v<decltype(T::generated_func), regex_generated_function_type>;

        // 違反している場合、無限再帰が発生してしまう
        requires T::begin_pos < T::end_pos;
    };

    /**
     * @class
     * @brief 正規表現格納オブジェクト。動的に生成されたパターンについては考慮しない
     * @tparam Pttern 正規表現の文字列リテラルを指定
     * @tparam Parser 正規表現を解析するテンプレートクラスを指定
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
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

            auto parse_result = parse<parser::end_pos, N + 1>();
            parse_result[N] = parser::generated_func;
            return parse_result;
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
            return std::array<regex_generated_function_ptr_type, N>{nullptr};
        }

    public:
        static constexpr auto parse_result = parse<0, 0>();

        /**
         * @fn
         * @brief パターンマッチの基本。実行後の結果はPtternのsizeとなるが、一致しなかった場合はstd::string_view::nposが返される
        */
        static constexpr std::size_t run(const std::string_view& s, std::size_t offset = 0)
        {
            bool is_pos_lock = false;
            for(regex_generated_function_ptr_type before = nullptr; const auto& f : parse_result) {
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
