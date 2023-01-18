///----------------------------------
/// @file bracket.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief 開始or終了括弧を渡し、対応する括弧の情報とエラー内容を提供する
    */
    template <char BrancketChar, bool IsBegin>
    struct bracket_info
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
    struct bracket_inner
    {
        using bracket_info = bracket_info<Pattern[BrancketBeginPos], true>;

        // 開始括弧の位置
        static constexpr auto begin_pos = BrancketBeginPos;

        // 終わり括弧の位置
        static constexpr auto end_pos = []() {
            // 括弧の多重度で対応括弧か判定
            for (std::size_t depth{}; const auto i : std::views::iota(begin_pos, Pattern.size()))
                if (!eq_before_char(Pattern, i, '\\'))
                    switch (Pattern[i]) {
                        case bracket_info::begin:
                            depth++;
                            break;
                        case bracket_info::end:
                            if ((--depth) == 0) return i + 1;
                            break;
                    }
            // 対応括弧未発見
            return std::string_view::npos;
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
        static constexpr auto value_with_bracket = bracket_info::begin + value + bracket_info::end;
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP
