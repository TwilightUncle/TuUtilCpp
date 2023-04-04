///----------------------------------
/// @file regex/general.hpp 
/// @brief regex.hpp�̊֘A
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP

namespace tuutil::str::_regex
{
    /**
     * @class
     * @brief �ʏ�̕�����̃}�b�`���O(�Ƃ����ʂ̂悤�ȒP�̂ő��݂��Ȃ��҂̌��؂�������)
    */
    template <cstr Pattern, std::size_t Pos>
    struct general
        : public regex_char_attribute
    {
        static constexpr auto begin_pos = Pos;

        static constexpr auto end_pos = []() {
            for (auto i = Pos; i < Pattern.size(); i++) {
                // �擪�����ꕶ���̎��̓G�X�P�[�v����Ă���Ƒz��
                if (attributes[Pattern[i]] && i == Pos) continue;

                // �񕶎��ڈȍ~�̓��ꕶ��
                if (attributes[Pattern[i]] && !(attributes[Pattern[i]] & BK)) {
                    // ���ʎ��͒��O�̂P�����݂̂ɓK�p����邽�߁A��C�ɔ��肷�镶���񂩂�͏��O����
                    if (i - Pos > 1 && (attributes[Pattern[i]] & QUANTIFIER)) return i - 1;
                    return i;
                }
            }
            return Pattern.size();
        }();

        static constexpr auto value = Pattern.template substr<Pos, end_pos - Pos>();

        /**
         * @fn
         * @brief ��͌��ʐ������ꂽ����
        */
        template <std::size_t N>
        static constexpr regex_match_result generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            auto pos = is_pos_lock
                ? (exists_in_position(value, s, offset) ? offset : std::string_view::npos)
                : find(value, s, offset);
            return (pos == std::string_view::npos)
                ? regex_match_result::make_unmatch()
                : regex_match_result{pos, value.size()};
        }
    };
}

#endif //TUUTILCPP_INCLUDE_GUARD_STR_REGEX_GENERAL_HPP
