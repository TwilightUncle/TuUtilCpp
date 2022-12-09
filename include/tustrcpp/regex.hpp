///----------------------------------
/// @file regex.hpp 
/// @brief 定数式に置ける正規表現(ユニコード未対応)
///----------------------------------
#pragma once
#include <regex>
#include <tustrcpp/cstr.hpp>

namespace tustr
{
    enum {
        // 通常範囲に置ける特殊文字の性質
        REGEX_SPECIAL       = 0x0001u,  // 特殊文字(バックスラッシュでエスケープ)
        REGEX_BK_SPECIAL     = 0x0002u,  // 特殊文字(バックスラッシュを付与した場合)
        REGEX_BRANCKET_BE   = 0x0004u,  // 括弧始まり
        REGEX_BRANCKET_EN   = 0x0008u,  // 括弧終わり
        REGEX_PREV_REF      = 0x0010u,  // 前の文字を参照する
        REGEX_NEXT_REF      = 0x0020u,  // 次の文字を参照する
        REGEX_BEGIN         = 0x0040u,  // 先頭にしか指定できない
        REGEX_END           = 0x0080u,  // 末尾にしか指定できない

        // []内における特殊文字の性質
        // REGEX_CLASSES_SPECIAL以外にビットがったていなければバックスラッシュでエスケープが必要、
        // REGEX_CLASSES_SPECIAL以外にビットが立っている場合、該当する全てを満たしていない場合では特殊文字として認識してはいけない
        REGEX_CLASSES_SPECIAL       = 0x0100u,  // 特殊文字([]内における)
        REGEX_CLASSES_BK_SPECIAL    = 0x0200u,
        REGEX_CLASSES_PREV_REF      = 0x0400u,
        REGEX_CLASSES_NEXT_REF      = 0x0800u,
        REGEX_CLASSES_BEGIN         = 0x1000u,  // 先頭
        REGEX_CLASSES_END           = 0x2000u,

        // 特殊文字かどうかの判定用マスク
        REGEX_SPECIAL_MASK = REGEX_SPECIAL | REGEX_BK_SPECIAL | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BK_SPECIAL,
    };

    /**
     * @fn
     * @brief 指定した文字の正規表現としての文法的な性質を示す属性のビット列を取得する
     * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions/Cheatsheet
    */
    inline constexpr auto get_regex_character_attribute(char ch)
    {
        std::uint32_t attrs[(std::numeric_limits<char>::max)() + 1] = {};
        // Character classes
        attrs['['] |= REGEX_SPECIAL | REGEX_BRANCKET_BE | REGEX_CLASSES_SPECIAL;
        attrs[']'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN | REGEX_CLASSES_SPECIAL;
        attrs['-'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_PREV_REF | REGEX_CLASSES_NEXT_REF;
        attrs['^'] |= REGEX_CLASSES_SPECIAL | REGEX_CLASSES_BEGIN;
        attrs['.'] |= REGEX_SPECIAL;
        attrs['d'] |= REGEX_BK_SPECIAL;
        attrs['D'] |= REGEX_BK_SPECIAL;
        attrs['w'] |= REGEX_BK_SPECIAL;
        attrs['W'] |= REGEX_BK_SPECIAL;
        attrs['s'] |= REGEX_BK_SPECIAL;
        attrs['S'] |= REGEX_BK_SPECIAL;
        attrs['t'] |= REGEX_BK_SPECIAL;
        attrs['r'] |= REGEX_BK_SPECIAL;
        attrs['n'] |= REGEX_BK_SPECIAL;
        attrs['v'] |= REGEX_BK_SPECIAL;
        attrs['f'] |= REGEX_BK_SPECIAL;
        attrs['b'] |= REGEX_CLASSES_BK_SPECIAL;
        attrs['0'] |= REGEX_BK_SPECIAL;
        attrs['c'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['x'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['u'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['\\'] |= REGEX_SPECIAL | REGEX_NEXT_REF | REGEX_CLASSES_SPECIAL | REGEX_CLASSES_NEXT_REF;
        attrs['|'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Assertions
        attrs['^'] |= REGEX_SPECIAL | REGEX_BEGIN;
        attrs['$'] |= REGEX_SPECIAL | REGEX_END;
        attrs['b'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['B'] |= REGEX_BK_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF | REGEX_NEXT_REF;
        // Groups and backreferences
        attrs['('] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs[')'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        for (const auto c : std::views::iota('1', '9'))
            attrs[c] |= REGEX_BK_SPECIAL;
        attrs['k'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        // Quantifiers
        attrs['*'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['+'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['?'] |= REGEX_SPECIAL | REGEX_PREV_REF;
        attrs['{'] |= REGEX_SPECIAL | REGEX_BRANCKET_BE;
        attrs['}'] |= REGEX_SPECIAL | REGEX_BRANCKET_EN;
        // Unicode property escapes
        attrs['p'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;
        attrs['P'] |= REGEX_BK_SPECIAL | REGEX_NEXT_REF;

        return attrs[ch];
    }

    struct regex_char_attribute
    {
        // 数量子の起点
        static constexpr auto quantifier_chars = cstr{"*+?{"};

        // アンカーの起点
        static constexpr auto anchor_chars = cstr{"^$("};
        static constexpr auto bk_anchor_chars = cstr{"bB"};

        // 文字クラス
        static constexpr auto class_chars = cstr{"."};
        static constexpr auto bk_class_chars = cstr{"dDwWsStrnvf0\\"};

        // (キャプチャ/非キャプチャ)グループ
        static constexpr auto capture_chars = cstr{"("};

        // or
        static constexpr auto or_match_chars = cstr{"|"};

        // 文字集合
        static constexpr auto char_set = cstr{"["};

        // キャプチャグループ参照
        static constexpr auto bk_reference_chars = cstr{"123456789"};
    };

    /**
     * @fn
     * @brief 正しくない書き方がされているバックスラッシュが存在する場合偽が返る
    */
    inline constexpr bool is_collect_regex_back_slash(const std::string_view& target)
    {
        for (int i = 0; i < target.size(); i++)
            if (target[i] == '\\' && (i == target.size() - 1 || !get_regex_character_attribute(target[++i])))
                return false;
        return true;
    }

    /**
     * @fn
     * @brief 許可リストまたは拒否リストとして指定したchar_listを参照し、targetが有効か検証する。バックスラッシュを含む場合、二つで一文字と認識する
     * @param target 検証する文字列
     * @param allow_or_deny allowの場合trueを指定、denyの場合falseを指定
     * @param char_list 許可または拒否文字のリスト。allow_or_denyによって拒否リストか許可リストか制御する
    */
    inline constexpr bool is_allowed_string(
        const std::string& target,
        bool allow_or_deny,
        const std::string& char_list,
        const std::string& bk_char_list,
        bool is_bk_escape = true
    ) {
        const auto collate_list = [&allow_or_deny](const std::string& list, char ch)->bool {
            // 許可リストの中から未発見のものがあった場合または、
            // 拒否リストの中から発見されたものがあった場合、失敗
            return (list.find_first_of(ch) == std::string::npos) == allow_or_deny;
        };

        for (int i = 0; i < target.size(); i++) {
            if ((target[i] != '\\' || !is_bk_escape) && collate_list(char_list, target[i])) return false;
            else if (target[i] == '\\' && i < target.size() - 1 && collate_list(bk_char_list, target[++i])) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief indexの位置の一つ前の文字が指定の文字か確認する
    */
    inline constexpr bool eq_before_char(const std::string_view& target, int index, char ch)
    {
        return index > 0 && target[(std::max)(index - 1, 0)] == ch;
    }

    /**
     * @class
     * @brief 開始or終了括弧を渡し、対応する括弧の情報とエラー内容を提供する
    */
    template <char BrancketChar, bool IsBegin>
    struct regex_bracket_info
    {
        // 開始括弧の文字
        static constexpr auto begin = []() {
            if (IsBegin) return BrancketChar;
            switch (BrancketChar) {
                case ']': return '[';
                case ')': return '(';
                case '}': return '{';
                case '>': return '<';
            }
            return char(-1);
        }(); 

        // 終了括弧の文字
        static constexpr auto end = []() {
            if (!IsBegin) return BrancketChar;
            switch (BrancketChar) {
                case '[': return ']';
                case '(': return ')';
                case '{': return '}';
                case '<': return '>';
            }
            return char(-1);
        }();

        // 引数が不正なので、コンパイルエラーを起こす
        static_assert(begin != -1, "Invalid template argment [BrancketChar]. Must match one of ']', ')', '}', '>'.");
        static_assert(end != -1, "Invalid template argment [BrancketChar]. Must match one of '[', '(', '{', '<'.");

        // 関連エラー発生時の識別子
        static constexpr auto error = []() {
            switch (begin) {
                case '[': return std::regex_constants::error_brack;
                case '(': return std::regex_constants::error_paren;
                case '{': return std::regex_constants::error_brace;
                case '<': return std::regex_constants::error_collate;
            }
            return std::regex_constants::error_type{};
        }();
    };

    template <cstr Pattern, std::size_t BrancketBeginPos>
    struct regex_bracket_inner
    {
        using bracket_info = regex_bracket_info<Pattern[BrancketBeginPos], true>;

        // 開始括弧の位置
        static constexpr auto begin_pos = BrancketBeginPos;

        // 終わり括弧の位置
        static constexpr auto end_pos = []() {
            const auto bracket_str = Pattern.view().substr(begin_pos + 1);
            auto pos = std::string_view::npos;
            for (const auto i : std::views::iota((std::size_t)0, bracket_str.size())) {
                // とじ括弧かつエスケープされていない場合、位置を記録し終了
                if (bracket_str[i] == bracket_info::end && !eq_before_char(bracket_str, i, '\\')) {
                    // substrする前のtargetの位置基準
                    pos = i + begin_pos + 2;
                    break;
                }
            }
            return pos;
        }();

        // エラーが発生したかどうか
        static constexpr auto is_error = end_pos == std::string_view::npos;
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_brack, "An error has occurred. [std::regex_constants::error_brack]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_paren, "An error has occurred. [std::regex_constants::error_paren]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_brace, "An error has occurred. [std::regex_constants::error_brace]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_collate, "An error has occurred. [std::regex_constants::error_collate]");

        // 括弧の中身
        static constexpr auto value = Pattern.substr<begin_pos + 1, end_pos - begin_pos - 2>();

        // 開始、閉じ括弧も含めた文字列
        static constexpr auto value_with_bracket = concat(char_to_cstr(bracket_info::begin), value, char_to_cstr(bracket_info::end));
    };

    struct regex_char_class
    {
    private:
        static constexpr auto digits = cstr{"0123456789"};
        static constexpr auto words = cstr{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"};
        static constexpr auto space = cstr{"\f\t\v"};
        static constexpr auto strends = cstr{"\n\r"};
        static constexpr auto bk_space = cstr{"ftv"};
        static constexpr auto bk_strends = cstr{"nr"};
        static constexpr auto others = cstr{"-!\"#$%&'()=^~|`@{}+;*:,<.>/?\b\\"};
        static constexpr auto bk_others = cstr{"b0[]\\"};

        static constexpr auto true_words = concat(digits, words);
        static constexpr auto true_space = concat(space, strends);
        static constexpr auto bk_true_space = concat(bk_space, bk_strends);
        static constexpr auto dot = concat(digits, words, space, others);
        static constexpr auto bk_dot = concat(bk_space, bk_others);
        static constexpr auto anti_digits = concat(words, space, strends, others);
        static constexpr auto bk_anti_digits = concat(bk_space, bk_strends, bk_others);
        static constexpr auto anti_words = concat(space, strends, others);
        static constexpr auto bk_anti_words = concat(bk_space, bk_others, bk_others);
        static constexpr auto anti_space = concat(digits, words, others);

    public:
        // 特殊文字に該当する文字集合を取得する
        inline static constexpr auto get_char_set(const char ch)
        {
            switch (ch) {
                case 'd': return std::pair{digits.view(), std::string_view{}};
                case 'w': return std::pair{true_words.view(), std::string_view{}};
                case 's': return std::pair{true_space.view(), bk_true_space.view()};
                case '.': return std::pair{dot.view(), bk_dot.view()};
                case 'D': return std::pair{anti_digits.view(), bk_anti_digits.view()};
                case 'W': return std::pair{anti_words.view(), bk_anti_words.view()};
                case 'S': return std::pair{anti_space.view(), bk_others.view()};
            }
            return std::pair{std::string_view{}, std::string_view{}};
        }

        template <char C>
        static constexpr auto get_const_char_set()
        {
            if constexpr (C == 'd') return digits;
            else if constexpr (C == 'w') return true_words;
            else if constexpr (C == 's') return true_space;
            else if constexpr (C == '.') return dot;
            else if constexpr (C == 'D') return anti_digits;
            else if constexpr (C == 'W') return anti_words;
            else if constexpr (C == 'S') return anti_space;
            else if constexpr (C == 'f') return cstr{"\f"};
            else if constexpr (C == 't') return cstr{"\t"};
            else if constexpr (C == 'v') return cstr{"\v"};
            else if constexpr (C == 'n') return cstr{"\n"};
            else if constexpr (C == 'r') return cstr{"\r"};
            else if constexpr (C == 'b') return cstr{"\b"};
            else return char_to_cstr(C);
        }

        template <char C>
        static constexpr auto get_const_bk_char_set()
        {
            if constexpr (C == 'd') return cstr{""};
            else if constexpr (C == 'w') return cstr{""};
            else if constexpr (C == 's') return bk_true_space;
            else if constexpr (C == '.') return bk_dot;
            else if constexpr (C == 'D') return bk_anti_digits;
            else if constexpr (C == 'W') return bk_anti_words;
            else if constexpr (C == 'S') return bk_others;
            else return char_to_cstr(C);
        }
    };

    template <cstr Pattern, std::size_t Pos>
    requires (Pos > 0)
    struct regex_quantifier_perser
        : public regex_char_attribute
    {
        static_assert(
            is_allowed_string(std::string{Pattern[Pos]}, true, quantifier_chars, "", false),
            "Invalied template argment [Pattern, Pos]. Must specified of '*', '+', '?', '{'."
        );

        // 数量子の開始位置
        static constexpr auto begin_pos = Pos;

        /**
         * @fn
         * @brief 数量子を示す文字列を抽出
        */
        template <char C> requires (C == '{')
        static constexpr auto extract_quantifier()
        {
            using bracket_inner = regex_bracket_inner<Pattern, Pos>;
            constexpr auto inner_str = bracket_inner::value;

            // 数量子の構文チェック
            // {n}, {n,}, {n,m}のいずれかの構文に合致していることを確認(n, mは数値であること)
            static_assert(
                inner_str[0] != ','
                && is_allowed_string(inner_str.data(), true, "0123456789,", "", false)
                && std::ranges::count(inner_str, ',') <= 1,
                "An error has occurred. [regex_quantifier_perser]"
            );

            return bracket_inner::value_with_bracket;
        }
        template <char C> requires (C != '{')
        static constexpr auto extract_quantifier() { return char_to_cstr(C); }

        // 数量子の文字列を抽出したもの
        static constexpr auto quantifier = extract_quantifier<Pattern[Pos]>();

        // 非貪欲の場合真
        static constexpr bool negative = []() {
            const auto check_pos = Pos + quantifier.size();
            return check_pos < Pattern.size() && Pattern[check_pos] == '?';
        }();

        // 数量子の末尾の次の位置
        static constexpr auto end_pos = quantifier.size() + std::size_t(negative);

        // 最小繰り返し回数
        static constexpr auto min_count = []()->std::size_t {
            if constexpr (quantifier[0] == '{') {
                constexpr auto inner_str = quantifier.remove_prefix_suffix<1, 1>();
                return to_int<std::size_t>(split<inner_str, ",">()[0]);
            }
            else {
                if (quantifier[0] == '+') return 1;
                return 0;
            }
        }();

        // 最大繰り返し回数
        static constexpr auto max_count = []()->std::size_t {
            if constexpr (quantifier[0] == '?') return 1;
            else if constexpr (quantifier[0] == '{') {
                constexpr auto devides = split<quantifier.remove_prefix_suffix<1, 1>(), ",">();
                if (devides.size() == 1) return to_int<std::size_t>(devides[0]);
                else if (!devides[1].empty()) return to_int<std::size_t>(devides[1]);
            }
            return std::string_view::npos;
        }();
    };

    template <cstr Pattern, std::size_t Pos>
    struct regex_range_parser
    {
        using brancket_inner = regex_bracket_inner<Pattern, Pos>;
        static constexpr auto char_range = brancket_inner::value;

        static_assert(char_range.size() > 0 && is_collect_regex_back_slash(char_range.view()));

        static constexpr auto allow_or_deny = char_range[0] != '^';
        static constexpr auto begin_index = int(!allow_or_deny);
        static constexpr auto size = char_range.size();
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = brancket_inner::end_pos;

        /**
         * @fn
         * @brief 文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else return concat(char_range.substr<N, 1>(), make_regex_char_list<N + 1>());
        }
        template <int N>
        requires (char_range[N] == '-')
        static constexpr auto make_regex_char_list()
        {
            if constexpr (begin_index == N || size - 1 == N)
                return concat(cstr{"-"}, make_regex_char_list<N + 1>());
            else {
                constexpr char begin_char = (std::min)(char_range[N - 1], char_range[N + 1]) + 1;
                constexpr char end_char = (std::max)(char_range[N - 1], char_range[N + 1]);
                cstr<2 + end_char - begin_char> str{};
                for (char c = 0; c < str.max_size; c++) str[c] = c + begin_char;
                return concat(str, make_regex_char_list<N + 2>());
            }
        }
        template <int N>
        requires (char_range[N] == '\\')
        static constexpr auto make_regex_char_list()
        {
            // 文字範囲の括弧内では「.」は特殊文字ではない
            if constexpr (char_range[N + 1] != '.')
                return concat(regex_char_class::get_const_char_set<char_range[N + 1]>(), make_regex_char_list<N + 2>());
            else return concat(cstr{"."}, make_regex_char_list<N + 2>());
        }

        /**
         * @fn
         * @brief バックスラッシュとセットの場合の文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_bk_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else if constexpr (char_range[N] == '\\' && char_range[N + 1] != '.')
                return concat(regex_char_class::get_const_bk_char_set<char_range[N + 1]>(), make_regex_bk_char_list<N + 2>());
            else return make_regex_bk_char_list<N + 1>();
        }

        // 文字集合
        static constexpr auto value = make_regex_char_list<begin_index>();

        // エスケープされた文字に合致する文字集合
        static constexpr auto bk_value = make_regex_bk_char_list<begin_index>();
    };

    /**
     * @fn
     * @brief 渡された文字範囲を許可/拒否リストに展開し、与えられた文字列がマッチするか判定する関数オブジェクトを返却する
    */
    template <cstr Pattern, std::size_t Pos>
    constexpr auto get_regex_char_range_matcher()
    {
        return [](const std::string& comp) -> bool {
            using range_parser = regex_range_parser<Pattern, Pos>;
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
