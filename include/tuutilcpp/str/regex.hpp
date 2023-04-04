///----------------------------------
/// @file str/regex.hpp 
/// @brief �萔���ɒu���鐳�K�\��(���j�R�[�h���Ή�)
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_REGEX_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_REGEX_HPP

#include <regex>
#include <tuutilcpp/str/regex/result.hpp>

namespace tuutil::str
{
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

        // �ᔽ���Ă���ꍇ�A�����ċA���������Ă��܂�
        requires T::begin_pos < T::end_pos;
    };
    
    // �O���錾
    template <cstr Pattern, std::size_t Pos> struct regex_parser;
    template <cstr Pattern, template <cstr, std::size_t> class Parser> struct regex;

    template <class T> struct is_regex : public std::false_type {};
    template <cstr Pattern, template <cstr, std::size_t> class Parser>
    struct is_regex<regex<Pattern, Parser>> : public std::true_type {};

    // �ċA�I�ɐ��K�\���N���X�����Ă��邩
    template <class T>
    concept InnerRegexReferable = requires {
        // �L���v�`���O���[�v�����ʓ����ċA�I�ɉ�͂��Ă���ꍇ
        typename T::inner_regex;
        requires is_regex<typename T::inner_regex>::value;
    };

    // �L���v�`�����s�����̂�
    template <class T>
    concept RegexParserCaptureable = requires {
        { T::is_capture } -> std::convertible_to<bool>;
        requires T::is_capture;
    };
}

#include <tuutilcpp/str/regex/common.hpp>
#include <tuutilcpp/str/regex/bracket.hpp>
#include <tuutilcpp/str/regex/char_class.hpp>
#include <tuutilcpp/str/regex/char_set.hpp>
#include <tuutilcpp/str/regex/capture.hpp>
#include <tuutilcpp/str/regex/general.hpp>
#include <tuutilcpp/str/regex/reference.hpp>
#include <tuutilcpp/str/regex/or.hpp>
#include <tuutilcpp/str/regex/assertion.hpp>
#include <tuutilcpp/str/regex/quantifier.hpp>
#include <tuutilcpp/str/regex/perser.hpp>

namespace tuutil::str
{
    /**
     * @class
     * @brief ���K�\���i�[�I�u�W�F�N�g�B���I�ɐ������ꂽ�p�^�[���ɂ��Ă͍l�����Ȃ��B�C���X�^���X�ɂ̓p�^�[���}�b�`�̌��ʂ��i�[�����B
     * @tparam Pattern ���K�\���̕����񃊃e�������w��
     * @tparam Parser ���K�\������͂���e���v���[�g�N���X���w��(option)
    */
    template <cstr Pattern, template <cstr, std::size_t> class Parser = regex_parser>
    struct regex
    {
        // ���@�エ�������o�b�N�X���b�V���̏o���͂����Ō��o�A�G���[�Ƃ���
        static_assert(is_collect_regex_back_slash(Pattern.view()));

        using parser = Parser<Pattern, 0>;
        using capture_store_type = regex_capture_store<parser::max_capture_count>;

        static constexpr auto exec(std::string_view s, std::size_t offset = 0, bool is_pos_lock = false)
        {
            auto cs = capture_store_type{};
            auto re = parser::exec<parser::max_capture_count>(s, offset, is_pos_lock, cs);
            return std::pair{cs, re};
        }

    private:
        // ���ʃL���v�`�����X�g���i�[
        capture_store_type capture_list;
        // ���ʂ̃}�b�`�͈͂��i�[
        regex_match_result match_result;
        // �e�X�g�Ώ�
        std::string_view test_target;
        // �e�X�g�Ώە�����̃I���W�i��(���s���̂ݎg�p)
        std::optional<std::string> test_target_origin;

    public:
        /**
         * @fn
         * @brief �p�^�[���}�b�`���s���A�C���X�^���X�ɂ͌��ʂ��i�[����
         * @param test_target �p�^�[���}�b�`�̑Ώە�����
         * @param offset �Ώە�����̃p�^�[���}�b�`���J�n����ʒu
         * @param is_pos_lock �^�̏ꍇ�p�^�[���}�b�`�̈ʒu��offset�ŌŒ肷��Boffset�̈ʒu�����v���Ă��Ȃ��ꍇ��v�Ȃ��ƂȂ�
        */
        constexpr regex(std::string_view test_target, std::size_t offset = 0, bool is_pos_lock = false)
        {
            // ���s���̂݁Astring_view�̎Q�Ƃɂ��A�N�Z�X�G���[�΍�̂��߁Astd::string�Ƀe�X�g�Ώۂ��R�s�[����
            if (!std::is_constant_evaluated()) {
                this->test_target_origin = std::string(test_target.begin(), test_target.end());
                this->test_target = std::string_view(this->test_target_origin.value());
            } else {
                this->test_target = test_target;
            }

            const auto [cs, mr] = exec(this->test_target, offset, is_pos_lock);
            this->capture_list = cs;
            this->match_result = mr;
        }

        /**
         * @fn
         * @brief �����Ώە�����Ƀp�^�[���}�b�`���Ă��镔�������݂�����^
        */
        constexpr bool exists() const noexcept { return bool(match_result); }

        /**
         * @fn
         * @brief �����Ώۂ̕�����S�̂��p�^�[���}�b�`���Ă���Ƃ��^
        */
        constexpr bool is_match() const noexcept { return this->exists() && match_result.match_length() == test_target.size(); }

        constexpr bool empty() const noexcept { return !this->exists(); }
        constexpr std::size_t size() const noexcept { return this->exists() ? this->capture_list.size() + 1 : 0; }

        /**
         * @fn
         * @brief �p�^�[���}�b�`���������̕�������擾����B���o���ʂȂ��̏ꍇ�󕶎���ԋp
         * @param index 0���w�肷��ƁA��v���S�́A1�ȏ�̒l��ݒ肷��ƊY������L���v�`���̃}�b�`���擾
        */
        constexpr std::string_view get_match_string_view(std::size_t index = 0) const
        {
            if (!this->exists() || this->capture_list.size() < index) return std::string_view{};
            if (!index) return test_target.substr(match_result.get_begin_pos(), match_result.match_length());
            return this->capture_list.get(index - 1);
        }

        constexpr std::string_view operator[](std::size_t n) const { return this->get_match_string_view(n); }

        /**
         * @fn
         * @brief �����̕�������ɁA�p�^�[���}�b�`���镔�������݂��Ă���ΐ^
        */
        static constexpr bool search(std::string_view s) { return regex(s).exists(); }

        /**
         * @fn
         * @brief �����̕�����̑S�̂��}�b�`���Ă���ꍇ�^
        */
        static constexpr bool match(std::string_view s) { return regex(s).is_match(); }
    };

    using empty_regex = regex<"">;
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_REGEX_HPP
