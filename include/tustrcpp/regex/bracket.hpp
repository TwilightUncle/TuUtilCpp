///----------------------------------
/// @file bracket.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_BRACKET_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_BRACKET_HPP

namespace tustr
{
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
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_BRACKET_HPP
