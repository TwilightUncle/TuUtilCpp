///----------------------------------
/// @file bracket.hpp 
/// @brief regex.hpp�֘A
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief �J�nor�I�����ʂ�n���A�Ή����銇�ʂ̏��ƃG���[���e��񋟂���
    */
    template <char BrancketChar, bool IsBegin>
    struct bracket_info
    {
        // �J�n���ʂ̕���
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

        // �I�����ʂ̕���
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

        // �������s���Ȃ̂ŁA�R���p�C���G���[���N����
        static_assert(begin != -1, "Invalid template argment [BrancketChar]. Must match one of ']', ')', '}', '>'.");
        static_assert(end != -1, "Invalid template argment [BrancketChar]. Must match one of '[', '(', '{', '<'.");

        // �֘A�G���[�������̎��ʎq
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

        // �J�n���ʂ̈ʒu
        static constexpr auto begin_pos = BrancketBeginPos;

        // �I��芇�ʂ̈ʒu
        static constexpr auto end_pos = []() {
            // ���ʂ̑��d�x�őΉ����ʂ�����
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
            // �Ή����ʖ�����
            return std::string_view::npos;
        }();

        // �G���[�������������ǂ���
        static constexpr auto is_error = end_pos == std::string_view::npos;
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_brack, "An error has occurred. [std::regex_constants::error_brack]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_paren, "An error has occurred. [std::regex_constants::error_paren]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_brace, "An error has occurred. [std::regex_constants::error_brace]");
        static_assert(!is_error || bracket_info::error != std::regex_constants::error_collate, "An error has occurred. [std::regex_constants::error_collate]");

        // ���ʂ̒��g
        static constexpr auto value = Pattern.substr<begin_pos + 1, end_pos - begin_pos - 2>();

        // �J�n�A�����ʂ��܂߂�������
        static constexpr auto value_with_bracket = bracket_info::begin + value + bracket_info::end;
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_BRACKET_HPP
