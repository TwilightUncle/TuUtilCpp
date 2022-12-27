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
     * @class
     * @brief キャプチャ結果の保存と取得を行うオブジェクト。
    */
    template <std::size_t N>
    class regex_capture_store
    {
    private:
        std::array<std::string_view, N> buf{};
        std::size_t end_pos{};
    
    public:
        constexpr regex_capture_store() noexcept {}

        /**
         * @fn
         * @brief キャプチャ結果を格納
        */
        constexpr void push_back(std::string_view sv)
        {
            if (this->end_pos >= N) throw std::out_of_range(std::string("orver max size. max value is ") + std::to_string(N));
            this->buf[this->end_pos++] = sv;
        }

        template <std::size_t M>
        constexpr void push_back(const regex_capture_store<M>& cs)
        {
            for (auto i = std::size_t{}; this->end_pos < N && i < cs.size(); i++)
                this->push_back(cs.get(i));
        }

        /**
         * @fn
         * @brief 指定インデックスに格納されたキャプチャ内容を取り出す
        */
        constexpr auto get(std::size_t index) const
        {
            // 値が未格納の領域へのアクセスは禁止する
            if (index >= this->end_pos)
                throw std::out_of_range(
                    std::string("index exceeds maximum allowed value. size: ")
                    + std::to_string(this->end_pos) + ", specify index: " + std::to_string(index)
                );
            return this->buf[index];
        }

        constexpr auto front() const noexcept
        {
            static_assert(N, "don't call function. because [regex_capture_store<0>] max size is 0.");
            return this->buf[0];
        }
        constexpr auto back() const noexcept
        {
            static_assert(N, "don't call function. because [regex_capture_store<0>] max size is 0.");
            return this->buf[this->end_pos - 1];
        }

        constexpr bool empty() const noexcept { return !bool(this->end_pos); }
        constexpr std::size_t size() const noexcept { return this->end_pos; }
    };

    /**
     * @class
     * @brief 正規表現に一致した範囲を格納する
    */
    struct regex_match_range : public std::array<std::size_t, 2>
    {
        inline static constexpr auto make_unmatch() { return regex_match_range{std::string_view::npos, 0}; }

        inline constexpr bool is_match() const noexcept { return this->at(0) != std::string_view::npos; }
        inline constexpr explicit operator bool() const noexcept { return this->is_match(); }
        inline constexpr auto get_begin_pos() const noexcept { return this->at(0); }
        inline constexpr auto match_length() const noexcept { return this->at(1); }
        inline constexpr auto get_end_pos() const noexcept { return bool(*this) ? this->at(0) + this->at(1) : std::string_view::npos; }

        inline constexpr void set_begin_pos(std::size_t pos) noexcept { this->at(0) = pos; }
        inline constexpr void set_begin_pos(const regex_match_range& range) noexcept { this->at(0) = range[0]; }
        inline constexpr void set_match_length(std::size_t len) noexcept { this->at(1) = len; }
        inline constexpr void set_match_length(const regex_match_range& range) noexcept { this->at(1) = range[1]; }
        inline constexpr void set_end_pos(std::size_t pos) noexcept
        {
            if (pos == std::string_view::npos) (*this) = make_unmatch();
            else if (this->at(0) > pos) {
                this->at(0) = pos;
                this->at(1) = 0;
            }
            else this->at(1) = pos - this->at(0);
        }
        inline constexpr void set_end_pos(const regex_match_range& range) noexcept { this->set_end_pos(range.get_end_pos()); }
    };

    /**
     * @brief forやwhileで回せるように、関数へ変換する際の型
    */
    template <std::size_t N>
    using regex_generated_function_type = regex_match_range(std::string_view, std::size_t, bool, regex_capture_store<N>&);
    template <std::size_t N>
    using regex_generated_function_ptr_type = regex_match_range(*)(std::string_view, std::size_t, bool, regex_capture_store<N>&);

    template <class T>
    concept RegexParseable = requires {
        // 静的メンバが定義済みか
        T::begin_pos;
        T::end_pos;
        // T::generated_func<0>;

        // 上記メンバの型チェック
        requires std::is_same_v<decltype(T::begin_pos), const std::size_t>;
        requires std::is_same_v<decltype(T::end_pos), const std::size_t>;
        // requires is_regex_generated_function_type<decltype(T::generated_func)>::value; // 関数

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
        requires std::is_same_v<decltype(T::is_capture), const bool>;
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
#include <tustrcpp/regex/assertion.hpp>
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
     * @brief 正規表現格納オブジェクト。動的に生成されたパターンについては考慮しない。インスタンスにはパターンマッチの結果が格納される。
     * @tparam Pttern 正規表現の文字列リテラルを指定
     * @tparam Parser 正規表現を解析するテンプレートクラスを指定(option)
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
         * @brief 各解析結果における想定される最大のキャプチャ数を計算する
        */
        template <RegexParseable ParserPart>
        static consteval auto get_parser_max_capture_count()
        {
            std::size_t cnt{};
            // 再帰的にregexが定義されている場合
            if constexpr (InnerRegexReferable<ParserPart>)
                cnt += (ParserPart::max_count == std::string_view::npos)
                    ? allowed_max_capture_count
                    : ParserPart::inner_regex::max_capture_count * ParserPart::max_count;
            // 自身がキャプチャ
            if constexpr (RegexParserCaptureable<ParserPart>)
                cnt += (std::min)(allowed_max_capture_count, ParserPart::max_count);
            return cnt;
        }

        /**
         * @fn
         * @brief 正規表現の解析
        */
        template <std::size_t Pos, std::size_t N, std::size_t MaxCaptureCnt>
        static consteval auto parse()
        {
            // あらかじめ、Pattern[Pos]の値によって部分特殊化し、
            // 呼び出される処理をオーバーロードできるようにする
            using parser = Parser<Pattern, Pos>;

            static_assert(RegexParseable<parser>, "Invaild template argment [Parser]. See concept [RegexParseable].");

            auto [f_arr, capture_cnt] = parse<
                parser::end_pos,
                N + 1,
                (std::min)(MaxCaptureCnt + get_parser_max_capture_count<parser>(), allowed_max_capture_count)
            >();

            f_arr[N] = parser::generated_func;
            return std::pair{f_arr, capture_cnt};
        }

        /**
         * @fn
         * @brief 再帰の終端
        */
        template <std::size_t Pos, std::size_t N, std::size_t MaxCaptureCnt>
        requires (Pattern.size() <= Pos)
        static consteval auto parse()
        {
            // 各perserで以下関数ポインタに格納可能な関数を定義することで、
            // 解析結果を関数ポインタの配列として保持できるようにする
            return std::pair{
                std::array<regex_generated_function_ptr_type<MaxCaptureCnt>, N>{nullptr},
                MaxCaptureCnt
            };
        }

        // 解析を実行し、悔過を格納
        static constexpr auto parse_result = parse<0, 0, 0>();

    public:
        // 正規表現のパターンを解析した結果のルールを関数集合としたもの
        static constexpr auto& match_rules = parse_result.first;

        // 想定される最大キャプチャ数
        static constexpr auto max_capture_count = parse_result.second;

        using capture_store_type = regex_capture_store<max_capture_count>;
        using function_ptr_type = regex_generated_function_ptr_type<max_capture_count>;

        /**
         * @fn
         * @brief パターンマッチの基本。実行後の結果はPtternのsizeとなるが、一致しなかった場合はstd::string_view::nposが返される
        */
        static constexpr auto run(std::string_view s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            auto cs = capture_store_type{};
            auto re = regex_match_range::make_unmatch();
            for(const auto& f : match_rules) {
                const auto part_range = f(s, offset, std::exchange(is_pos_lock, true), cs);
                if (!part_range)
                    return std::pair{cs, regex_match_range::make_unmatch()};
                re.set_begin_pos((std::min)(part_range.get_begin_pos(), re.get_begin_pos()));
                offset = part_range.get_end_pos();
            }
            re.set_end_pos(offset);
            return std::pair{cs, re};
        }

        /**
         * @fn
         * @brief 引数の文字列内に、パターンマッチする部分が内包されていれば真
        */
        static constexpr bool search(std::string_view s) { return bool(run(s, 0).second); }

        /**
         * @fn
         * @brief 引数の文字列の全体がパターンマッチしている場合真
        */
        static constexpr bool match(std::string_view s) { return run(s, 0, true).second.match_length() == s.size(); }

    private:
        // 結果キャプチャリストを格納
        const capture_store_type capture_list;
        // 結果のマッチ範囲を格納
        const regex_match_range match_range;
        // テスト対象
        const std::string_view test_target;

        constexpr regex(std::string_view test_target, const std::pair<capture_store_type, regex_match_range>& run_result)
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
            : regex(test_target, run(test_target, offset, is_pos_lock))
        {}

        /**
         * @fn
         * @brief 検査対象文字列にパターンマッチしている部分が存在したら真
        */
        constexpr bool is_find() const noexcept { return bool(match_range); }

        /**
         * @fn
         * @brief 検査対象の文字列全体がパターンマッチしているとき真
        */
        constexpr bool is_match() const noexcept { return match_range.match_length() == test_target.size(); }

        /**
         * @fn
         * @brief パターンマッチした部分の文字列を取得する。抽出結果なしの場合空文字を返却
         * @param index 0を指定すると、一致個所全体、1以上の値を設定すると該当するキャプチャのマッチを取得
        */
        constexpr std::string_view get_match_part(std::size_t index = 0) const
        {
            if (!this->is_find() || this->capture_list.size() < index) return std::string_view{};
            if (!index) return test_target.substr(match_range.get_begin_pos(), match_range.match_length());
            return this->capture_list.get(index - 1);
        }
    };

    using empty_regex = regex<"">;
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_HPP
