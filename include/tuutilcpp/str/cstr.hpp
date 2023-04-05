///----------------------------------
/// @file str/cstr.hpp 
/// @brief �萔���ɒu���镶����
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP

namespace tuutil::str
{
    /**
     * @fn
     * @brief subject��pos�Ŏw�肵���ʒu��search�Ŏw�肵�������񂪑��݈ς��邩����
     * @param search �������镶����
     * @param subject �Ώە�����
     * @param pos ����ʒu
    */
    inline constexpr auto exists_in_position(
        std::string_view search,
        std::string_view subject,
        std::size_t pos
    ) {
        // ��������pos�ȍ~�̕��������Asearch���Z���ꍇ���݂̂��悤���Ȃ�
        if (search.size() > subject.size() - pos) return false;
        // ��v�̊m�F
        for (std::size_t i = 0; i < search.size(); i++) {
            if (subject[i + pos] != search[i]) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief subject�̒���search���܂܂�Ă��邩��������
     * @param search �������镶����
     * @param subject �Ώە�����
     * @param offset �����J�n�ʒu
    */
    inline constexpr auto find(
        std::string_view search,
        std::string_view subject,
        std::size_t offset = 0
    ) {
        for (std::size_t i = offset; i < subject.size(); i++)
            if (exists_in_position(search, subject, i)) return i;
        return std::string_view::npos;
    }

    /**
     * @class
     * @brief �Œ蒷�̈�̕�����
    */
    template <std::size_t N>
    struct cstr : public std::array<char, N>
    {
    private:
        using parent_type = std::array<char, N>;

    public:
        // �I�[�������i�[�ς݂ł�����̂Ƃ��Ĉ���
        static constexpr auto max_size = N - 1;

        constexpr cstr() : parent_type{} {}
        constexpr cstr(const char (&str_literal)[N])
            : parent_type{}
        {
            for (auto i = 0; i < max_size; i++)
                this->data()[i] = str_literal[i];
        }

        /**
         * @fn
         * @brief string_view���擾
        */
        constexpr auto view() const noexcept { return std::string_view{this->data()}; }
        constexpr operator std::string_view() const noexcept { return this->view(); }
        constexpr operator std::string() const noexcept { return std::string(this->data()); }

        /**
         * @fn
         * @brief �ێ����Ă��镶����̒������擾
        */
        constexpr auto size() const noexcept { return view().size(); }

        /**
         * @fn
         * @brief �ێ����Ă��镶���񂪋�̏ꍇ�^
        */
        constexpr auto empty() const noexcept { return view().empty(); }

        /**
         * @fn
         * @brief s�̕����񂪏o�����鐔���J�E���g
        */
        constexpr auto count(std::string_view s) const
        {
            const auto sv = this->view();
            std::size_t cnt = 0;
            for (int i = 0; i < sv.size(); i++)
                if (exists_in_position(s, sv, i)) cnt++;
            return cnt;
        }

        /**
         * @fn
         * @brief �����񂪍ŏ��ɏo������ꏊ����������
        */
        constexpr auto find(std::string_view s, std::size_t offset = 0) const { return str::find(s, this->view(), offset); }

        /**
         * @fn
         * @brief �w��̕����A�����񂪊܂܂��Ƃ��^
        */
        constexpr auto contains(std::string_view s) const { return this->find(s) != std::string_view::npos; }

        /**
         * @fn
         * @brief �����W�����������A�ŏ��Ɍ����������̈ʒu�ƁA��������A�����č��v�����������̃y�A��Ԃ�
         * @param char_set �����W��
         * @param offset �J�n�ʒu
         * @param is_allow false���w�肷��ƕ����W���Ƀ}�b�`���Ȃ��Ƃ��͈̔͂�Ԃ�
         * @return ������Ȃ��ꍇ�A�ʒu��std::string_view::npos
        */
        constexpr auto get_charset_match_range(std::string_view char_set, std::size_t offset = 0, bool is_allow = true) const
        {
            std::size_t s_pos = std::string_view::npos, cnt{};
            for (std::size_t i = offset; i < this->size(); i++) {
                const auto is_find = str::find(cstr{{this->at(i), '\0'}}, char_set) != std::string_view::npos;
                if (is_allow != is_find) break;
                s_pos = (std::min)(s_pos, i);
                cnt++;
            }
            return std::array{s_pos, cnt};
        }

        /**
         * @fn
         * @brief �����W�����������A�ŏ��Ɍ�����Ȃ����������̈ʒu�ƁA��������A�����č��v�������Ȃ��������̃y�A��Ԃ�
         * @param char_set �����W��
         * @param offset �J�n�ʒu
        */
        constexpr auto get_charset_unmatch_range(std::string_view char_set, std::size_t offset = 0) const
        {
            return this->get_charset_match_range(char_set, offset, false);
        }

        /**
         * @fn
         * @brief �S�Ă̗v�f���w�肳�ꂽ�����W���Ɋ܂܂�Ă���Ƃ��^
        */
        constexpr auto match_all_charset(std::string_view char_set, bool is_allow = true) const
        {
            auto [s_pos, cnt] = this->get_charset_match_range(char_set, 0, is_allow);
            return s_pos == 0 && cnt == this->size();
        }

        /**
         * @fn
         * @brief �S�Ă̗v�f���w�肵�������W���Ɋ܂܂�Ă��Ȃ��Ƃ��^
        */
        constexpr auto unmatch_all_charset(std::string_view char_set) const { return this->match_all_charset(char_set, false); }

        /**
         * @fn
         * @brief ������������擾����
        */
        template <std::size_t Offset, std::size_t Count = max_size>
        requires (Offset <= max_size)
        constexpr auto substr() const
        {
            constexpr auto len = (std::min)(Count, max_size - Offset);
            cstr<len + 1> _s{};
            for (auto i = 0; i < len; i++) _s[i] = this->data()[i + Offset];
            return _s;
        }

        /**
         * @fn
         * @brief �w�蕶�����A������̑O�ォ����
        */
        template <std::size_t Prefix, std::size_t Suffix>
        requires (Prefix + Suffix <= max_size)
        constexpr auto remove_prefix_suffix() const
        {
            return this->template substr<Prefix, max_size - Prefix - Suffix>();
        }

        /**
         * @fn
         * @brief �擪��N�������폜����
        */
        template <std::size_t Prefix>
        constexpr auto remove_prefix() const { return this->template remove_prefix_suffix<Prefix, 0>(); }

        /**
         * @fn
         * @brief ������N�������폜����
        */
        template <std::size_t Suffix>
        constexpr auto remove_suffix() const { return this->template remove_prefix_suffix<0, Suffix>(); }
    };

    /**
     * @fn
     * @brief �������璷��1�̕�������쐬
    */
    inline constexpr auto char_to_cstr(char ch) { return cstr{{ch, '\0'}}; }

    /**
     * @fn
     * @brief ���cstr�^���������������
    */
    template <std::size_t N1, std::size_t N2>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // s1�̏I�[�����͏������邽�߁A-1
        cstr<N1 + N2 - 1> _s{};
        for (int i = 0; i < N1 - 1; i++) _s[i] = s1[i];
        for (int i = 0; i < N2; i++) _s[i + N1 - 1] = s2[i];
        return _s;
    }

    /**
     * @fn
     * @brief n��cstr�^���������Ɍ�������
    */
    template <std::size_t N1, std::size_t N2, std::size_t... Sizes>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2, const cstr<Sizes>&... strs)
    {
        return concat(concat(s1, s2), strs...);
    }

    /**
     * @fn
     * @brief Offset����Count�̕���������V�K������Ƃ��Đ؂�o��
    */
    template <std::size_t Offset, std::size_t Count, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return s.template substr<Offset, Count>(); }
    // Offset���當����̍Ō���܂�
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return s.template substr<Offset>(); }

    /**
     * @fn
     * @brief target����؂蕶���ŕ����������ʂ�view�̔z��ŕԋp
    */
    template <cstr target, cstr delimiter>
    constexpr auto split()
    {
        constexpr auto size = target.count(delimiter);
        auto res = std::array<std::string_view, size + 1>{};
        auto sv = target.view();

        for (std::size_t pos{}, prev{}, i{}; (pos = target.find(delimiter, pos)) != std::string_view::npos; prev = pos) {
            res[i++] = sv.substr(0, pos - prev);
            sv = sv.substr((pos += delimiter.size()) - prev);
        }
        res[size] = sv;
        return res;
    }

    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() == s2.view(); }

    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() <=> s2.view(); }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const cstr<N1>& s1, const cstr<N2>& s2) { return concat(s1, s2); }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const char (&s1)[N1], const cstr<N2>& s2) { return cstr<N1>{s1} + s2; }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const cstr<N1>& s1, const char (&s2)[N2]) { return s1 + cstr<N2>{s2}; }

    template <std::size_t N>
    constexpr auto operator+(const char c, const cstr<N>& s) { return char_to_cstr(c) + s; }

    template <std::size_t N>
    constexpr auto operator+(const cstr<N>& s, const char c) { return s + char_to_cstr(c); }

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌Œ蒷��������A��؂蕶���������ň�̕����ւƌ�������
     * @param separator ��؂蕶����
     * @param head �p�����[�^�p�b�N�̐擪������(����str���ʂ���K�v�͂Ȃ�)
     * @param strs �Œ蒷������̃p�����[�^�p�b�N
    */
    template <std::size_t SepN, std::size_t HeadN, std::size_t... Sizes>
    constexpr auto join(const cstr<SepN>& separator, const cstr<HeadN>& head, const cstr<Sizes>&... strs)
    {
        return head + ((separator + strs) + ...);
    }

    template <std::size_t SepN, std::size_t... Sizes>
    constexpr auto join(const char (&separator)[SepN], const cstr<Sizes>&... strs) { return join(cstr<SepN>{separator}, strs...); }

    template <std::size_t... Sizes>
    constexpr auto join(const char c, const cstr<Sizes>&... strs) { return join(char_to_cstr(c), strs...); }

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌Œ蒷��������A��؂蕶���������ň�̕����ւƌ�������
     * @tparam Separator ��؂蕶����
     * @tparam List �Œ蒷������̃p�����[�^�p�b�N�����^
    */
    template <cstr Separator, class List> struct cstrs_join;
    template <cstr Separator, template <cstr...> class List, cstr... Strs>
    struct cstrs_join<Separator, List<Strs...>> : public mpl::value_constant<join(Separator, Strs...)> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌Œ蒷��������A��؂蕶���������ň�̕����ւƌ�������
     * @tparam Separator ��؂蕶����
     * @tparam Strs �Œ蒷������̃p�����[�^�p�b�N
    */
    template <cstr Separator, class List> constexpr auto cstrs_join_v = cstrs_join<Separator, List>::value;

    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l��cstr�ɕϊ�����B�i����n�����ƂŁA(2,8,16)�i�����e�����̂悤�ȕ�����ɕϊ�����
    */
    template <auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2 && (std::integral<decltype(V)> || std::convertible_to<decltype(V), utility::big_int<2>>))
    constexpr auto to_string()
    {
        // 10�i���Ƃ��Č������擾
        constexpr auto len = utility::get_digit<V, Hex>();
        // �I�[�����̒������z��̗v�f���Ɋ܂߂�
        cstr<len + 1> s{};
        auto val = V;
        for (auto i = len; i > 0; val /= Hex) {
            const auto code = static_cast<char>(val % Hex);
            s[--i] = (code < 10)
                ? '0' + code
                : 'A' + code - 10;
        }

        // �i�����e�����̐ړ��q��t�^
        if constexpr (UsePrefix) {
            if constexpr (Hex == 2) return "0b" + s;
            else if constexpr (Hex == 8) return "0" + s;
            else if constexpr (Hex == 16) return "0x" + s;
            else return s;
        }
        else return s;
    }
    // �����̏ꍇ
    template <std::integral auto V, std::size_t Hex = 10, bool UsePrefix = false>
    requires (V < 0)
    constexpr auto to_string() { return "-" + to_string<-V, Hex, UsePrefix>(); }

    /**
     * @fn
     * @brief ������𐮐��^�ɕϊ�
    */
    template <std::integral T>
    constexpr T to_int(std::string_view s)
    {
        T val{};
        for (const auto c: s) {
            if (c < '0' || c > '9') break;
            val *= 10;
            val += (c - '0');
        }
        return val;
    }

    /**
     * @fn
     * @brief cstr�ł��邩�̃��^�֐�
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP
