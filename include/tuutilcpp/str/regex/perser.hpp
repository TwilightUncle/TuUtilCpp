///----------------------------------
/// @file perser.hpp 
/// @brief regex.hpp�֘A(�p�^�[����͋@�\����̃N���X�ɏW�ς�������)
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_PERSER_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_PERSER_HPP

#include <tuutilcpp/str/regex/resolve.hpp>

namespace tuutil::str
{
    /**
     * @class
     * @brief ���K�\���p�^�[�����(or�̏ꍇ)
     * @tparam Pattern ���K�\���p�^�[���̕����񃊃e�������w��
     * @tparam Pos Pattern�̉�͂���J�n�ʒu���w��
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_parser
    {
        using parsed_type = _regex::or_parser<Pattern, _regex::get_or_pos_regex_pattern(Pattern)>;

        // �ÓI�ɗ̈���m�ۂ��邽�߁A���K�\���p�^�[������킩��ő�L���v�`�������擾
        static constexpr std::size_t max_capture_count = parsed_type::inner_regex::parser::max_capture_count;

        /**
         * @fn
         * @brief �p�^�[���}�b�`���O���s
         * @param subject ���ؑΏە�����
         * @param offset subject�̌��؊J�n�ʒu
         * @param is_pos_lock offset�̈ʒu�����v��������{����ꍇ�^�B�U���w�肵���ꍇoffset�ȍ~�̔C�ӂ̏ꏊ�ň�v���Ă��邩���肷��
         * @param capture_store �L���v�`�����e�̒ǉ��ƎQ�Ƃ����{����
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            return parsed_type::template generated_func<MaxCaptureCount>(subject, offset, is_pos_lock, capture_store);
        }
    };

    /**
     * @class
     * @brief ���K�\���p�^�[���̎w����̉�͂��s��(or�ȊO�̐��K�\������͂��邽�߂̓��ꉻ)
     * @tparam Pattern ���K�\���p�^�[���̕����񃊃e�������w��
     * @tparam Pos Pattern�̉�͂���J�n�ʒu���w��
    */
    template <cstr Pattern, std::size_t Pos>
    requires (_regex::get_or_pos_regex_pattern(Pattern) == std::string_view::npos && Pattern.size() > Pos)
    struct regex_parser<Pattern, Pos>
    {
        // �P�̂Ŏw�肵�Ă͂����Ȃ�����������
        static_assert(
            !(regex_char_attribute::attributes[Pattern[Pos]] & regex_char_attribute::DENY),
            "Invalied template argment [Pattern, Pos]. Must not specified of '}', ')', ']'."
        );

        using parsed_type = tuutil::mpl::apply_t<
            _regex::bind_regex_pattern_t<_regex::add_quantifier, Pattern>,
            _regex::resolve_parser<Pattern, Pos>
        >;

        // �ȍ~�̈ʒu�̉��
        using parsed_next_type = regex_parser<Pattern, parsed_type::end_pos>;

        // �ÓI�ɗ̈���m�ۂ��邽�߁A���K�\���p�^�[������킩��ő�L���v�`�������擾
        static constexpr std::size_t max_capture_count = []() {
            constexpr std::size_t allowed_max_capture_count = 65535;
            std::size_t cnt{};
            // �ċA�I��regex����`����Ă���ꍇ
            if constexpr (InnerRegexReferable<parsed_type>)
                cnt += (parsed_type::max_count == std::string_view::npos)
                    ? allowed_max_capture_count
                    : parsed_type::inner_regex::parser::max_capture_count * parsed_type::max_count;
            // ���g���L���v�`��
            if constexpr (RegexParserCaptureable<parsed_type>)
                cnt += (std::min)(allowed_max_capture_count, parsed_type::max_count);
            return (std::min)(allowed_max_capture_count, cnt + parsed_next_type::max_capture_count);
        }();

        /**
         * @fn
         * @brief �p�^�[���}�b�`���O���s�B���ʎ��̃��[�v�������Ő���
         * @param subject ���ؑΏە�����
         * @param offset subject�̌��؊J�n�ʒu
         * @param is_pos_lock offset�̈ʒu�����v��������{����ꍇ�^�B�U���w�肵���ꍇoffset�ȍ~�̔C�ӂ̏ꏊ�ň�v���Ă��邩���肷��
         * @param capture_store �L���v�`�����e�̒ǉ��ƎQ�Ƃ����{����
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>& capture_store)
        {
            std::size_t cnt = 0;
            regex_capture_store<MaxCaptureCount> temp_capture_store = capture_store, init_capture_store = capture_store;
            auto result = regex_match_result::make_unmatch();
            bool is_lock = is_pos_lock;

            for (auto end_pos = offset, begin_pos = std::string_view::npos; end_pos <= subject.size() && cnt < parsed_type::max_count;) {
                const auto temp_result = parsed_type::template generated_func<MaxCaptureCount>(
                    subject,
                    end_pos,
                    std::exchange(is_lock, true),
                    temp_capture_store
                );
                if (!temp_result) break;

                // ����ʂ����̂ōX�V
                begin_pos = (std::min)(begin_pos, temp_result.get_begin_pos());
                end_pos = temp_result.get_end_pos();

                if (++cnt >= parsed_type::min_count) {
                    // ���肪�^�̎��̂ݒl���X�V�����悤�A��U�ꎞ�ϐ��ɍX�V�l���L�^
                    auto cs = temp_capture_store;
                    const auto re = parsed_next_type::exec<MaxCaptureCount>(subject, end_pos, true, cs);
                    if (!re) continue;
                    result.set_begin_pos(begin_pos);
                    result.set_end_pos(re);
                    capture_store = cs;
                    if constexpr (!parsed_type::greedy) break;
                }
            }

            // 0��}�b�`�ł�OK�̏ꍇ
            if ((!cnt || !parsed_type::greedy) && !parsed_type::min_count)
                if (const auto re = parsed_next_type::exec<MaxCaptureCount>(subject, offset, is_pos_lock, init_capture_store)) {
                    result = re;
                    capture_store = init_capture_store;
                }

            return (cnt < parsed_type::min_count)
                    ? regex_match_result::make_unmatch()
                    : result;
        }
    };

    /**
     * @class
     * @brief �ċA�\���p�^�[�����(�ċA��`�̏I�_�̓��ꉻ)
     * @tparam Pattern ���K�\���p�^�[���̕����񃊃e�������w��
     * @tparam Pos Pattern�̉�͂���J�n�ʒu���w��
    */
    template <cstr Pattern, std::size_t Pos>
    requires (Pattern.size() <= Pos)
    struct regex_parser<Pattern, Pos>
    {
        // �ÓI�ɗ̈���m�ۂ��邽�߁A���K�\���p�^�[������킩��ő�L���v�`�������擾
        static constexpr std::size_t max_capture_count = 0;

        /**
         * @fn
         * @brief �p�^�[���}�b�`���O���s
         * @param subject ���ؑΏە�����
         * @param offset subject�̌��؊J�n�ʒu
         * @param is_pos_lock offset�̈ʒu�����v��������{����ꍇ�^�B�U���w�肵���ꍇoffset�ȍ~�̔C�ӂ̏ꏊ�ň�v���Ă��邩���肷��
         * @param capture_store �L���v�`�����e�̒ǉ��ƎQ�Ƃ����{����
        */
        template <std::size_t MaxCaptureCount>
        static constexpr auto exec(std::string_view subject, std::size_t offset, bool is_pos_lock, regex_capture_store<MaxCaptureCount>&)
        {
            return regex_match_result{offset, 0};
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_PERSER_HPP
