///----------------------------------
/// @file big_int.hpp
/// @brief 64 �~ Size�r�b�g�̐���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_UTILITY_BIG_INT_HPP
#define TUUTILCPP_INCLUDE_GUARD_UTILITY_BIG_INT_HPP

namespace tuutil::utility
{
    template <std::size_t Size = 2>
    requires (Size > 0)
    struct big_int : public carry_over_container<std::uint64_t, Size>
    {
        using value_type = std::uint64_t;

        // �����������Ă��Ȃ�
        bool sign = true;

        constexpr big_int() : carry_over_container<std::uint64_t, Size>{} {}

        constexpr big_int(std::integral auto i) : big_int()
        {
            this->at(0) = (i < 0) ? -i : i;
            this->sign = (i >= 0);
        }

        template <std::unsigned_integral... Types>
        requires (sizeof...(Types) == Size)
        constexpr big_int(Types... args)
            : carry_over_container<std::uint64_t, Size>{(value_type)args...}
        {}

        template <std::size_t N>
        constexpr big_int(const big_int<N>& v) : big_int()
        {
            for (std::size_t i = 0; i < (std::min)(Size, N); i++) this->at(i) = v[i];
        }

        static constexpr auto plus(value_type l_v, value_type r_v)
        {
            const auto is_carried = (std::numeric_limits<value_type>::max)() - l_v < r_v;
            return std::tuple{l_v + r_v, (value_type)is_carried, is_carried};
        }

        static constexpr auto minus(value_type l_v, value_type r_v)
        {
            const auto is_carried = (value_type)(l_v < r_v);
            const auto low = (std::numeric_limits<value_type>::max)() * is_carried - r_v + l_v + 1 * is_carried;
            return std::tuple{(value_type)low, is_carried, (bool)is_carried};
        }

        static constexpr auto mul(value_type l_v, value_type r_v)
        {
            constexpr auto harf_digits = std::numeric_limits<value_type>::digits / 2;
            constexpr auto harf_mask = ~0ull >> harf_digits;

            // ���������ꂼ��ő�r�b�g���̔����ŕ���
            const auto upper_l = l_v >> harf_digits;
            const auto lowwer_l = l_v & harf_mask;
            const auto upper_r = r_v >> harf_digits;
            const auto lowwer_r = r_v & harf_mask;

            // �������|��(�����Z����ہA���オ����l������K�v�����镔����plus��)
            const auto lowwer = lowwer_l * lowwer_r;
            const auto [middle_lower, middle_upper, _dust1] = plus(lowwer_l * upper_r, upper_l * lowwer_r);
            const auto upper = upper_l * upper_r;

            // �������|���̌��ʂ����v(�r�b�g���Z�q�Ɖ��Z�E���Z�ł̓r�b�g���Z�q�̂ق����D��x���Ⴂ�̂Œ���)
            const auto [lowwer_lower, lower_upper, _dust2] = plus(lowwer, middle_lower << harf_digits);
            const auto upper_result = upper
                + (middle_lower >> harf_digits)
                + (middle_upper << harf_digits)
                + lower_upper;

            return std::tuple{lowwer_lower, upper_result, upper_result > 0};
        }

        static constexpr auto lshift(value_type l_v, value_type r_v)
        {
            const auto high_rshfts = (std::numeric_limits<value_type>::digits - r_v) * (bool)r_v;
            const auto upper = (l_v >> high_rshfts) * (bool)r_v;
            return std::tuple{l_v << r_v, upper, upper > 0};
        }

        /**
         * @fn
         * @brief ��i���ɒu���錅�����擾
        */
        constexpr auto get_bit_digits() const noexcept
        {
            constexpr auto value_type_max_digits = std::numeric_limits<value_type>::digits;
            const auto digits64 = (int)count_using_size(*this);
            // �ŏ�ʗv�f�̍ŏ�ʃr�b�g
            const auto max_bit = value_type_max_digits - std::countl_zero(this->at(digits64 - 1u));
            return (std::max)(max_bit + (value_type_max_digits * (digits64 - 1)), 1);
        }

        constexpr big_int operator~() const
        {
            big_int result{*this};
            for (auto& v : result) v = ~v;
            return result;
        }

        constexpr operator value_type() const noexcept { return this->front(); }

        constexpr explicit operator bool() const noexcept
        {
            for (auto v : (*this))
                if (v) return true;
            return false;
        }

        constexpr bool operator!() const noexcept { return !static_cast<bool>(*this); }

        constexpr big_int& operator+=(const std::convertible_to<big_int> auto& v) { return this->template set_with_carry_up<big_int>(plus, v); }
        constexpr big_int& operator-=(const std::convertible_to<big_int> auto& v) { return this->template set_with_carry_up<big_int>(minus, v); }
        constexpr big_int& operator*=(const std::convertible_to<big_int> auto& v) { return this->template set_with_carry_up_all<big_int>(mul, v, plus); }

        constexpr big_int& operator<<=(std::unsigned_integral auto v)
        {
            constexpr auto value_type_max_digits = std::numeric_limits<value_type>::digits;
            return this->template set_with_carry_up_all<big_int>(
                lshift,
                v % value_type_max_digits,  // ���ۂɍs���V�t�g��
                plus,
                v / value_type_max_digits   // �V�t�g�Ō����܂�����
            );
        }

        constexpr big_int& operator>>=(std::unsigned_integral auto v)
        {
            constexpr auto value_type_max_digits = std::numeric_limits<value_type>::digits;
            const auto inverse_shift_value = (v % value_type_max_digits);
            return this->template set_with_carry_up_all<big_int>(
                lshift,
                // ���ۂɍs���V�t�g��
                (value_type_max_digits - inverse_shift_value) % value_type_max_digits,
                plus,
                // �V�t�g�Ō����܂�����
                -(v / value_type_max_digits + 1 * (bool)inverse_shift_value)
            );
        }

        /**
         * @fn
         * @brief ����v�Ŋ���Z���s���A���Ɨ]��̑g��ԋp����B�r�b�g���Z�ɂ�����
        */
        constexpr std::pair<big_int, big_int> div(const std::convertible_to<big_int> auto& v) const
        {
            // ������0�̏ꍇ�A�����I��0���Z���s��(�O�̂��߁A0/0��n/0����ʂ���)
            if (!v) const auto div_0 = (value_type)(bool)(*this) / value_type(v);

            constexpr auto value_type_max_digits = std::numeric_limits<value_type>::digits;
            auto rem = big_int{*this};
            auto quotient = big_int{};
            for (auto v_lshifts = this->get_bit_digits() - big_int{v}.get_bit_digits(); v_lshifts >= 0 && rem; v_lshifts--)
            {
                const auto shifted_v = (big_int{v} <<= (value_type)v_lshifts);
                // �������Ƃ������A���Z���ď��𗧂Ă�
                if (rem >= shifted_v) {
                    rem -= shifted_v;
                    quotient[v_lshifts / value_type_max_digits] |= (1ull << (v_lshifts % value_type_max_digits));
                }
            }
            return {quotient, rem};
        }

        constexpr big_int& operator/=(const std::convertible_to<big_int> auto& v) { return (*this) = this->div(v).first; }
        constexpr big_int& operator%=(const std::convertible_to<big_int> auto& v) { return (*this) = this->div(v).second; }

        constexpr big_int& operator++() { return (*this) += 1; }
        constexpr big_int operator++(int) { return std::exchange(*this, ++big_int{*this}); }
    };

    template <class T> struct is_big_int : public std::false_type {};
    template <std::size_t N> struct is_big_int<big_int<N>> : public std::true_type {};
    template <class T> constexpr auto is_big_int_v = is_big_int<T>::value;

    /**
     * @fn
     * @brief ����v�̌^���A��̈����̂����T�C�Y���傫���ق��̌^�ɕϊ����ĕԋp
    */
    template <std::size_t N>
    constexpr auto convert_max_size_type_value(const big_int<N>& v, std::integral auto) { return big_int<N>(v); }
    template <std::size_t N1, std::size_t N2>
    constexpr auto convert_max_size_type_value(const big_int<N1>& v, const big_int<N2>&) { return big_int<(std::max)(N1, N2)>(v); }

    template <class T1, class T2> struct get_big_int_max_size;
    template <std::size_t N1, std::size_t N2> struct get_big_int_max_size<big_int<N1>, big_int<N2>> : public std::integral_constant<std::size_t, (std::max)(N1, N2)> {};
    template <std::size_t N, std::integral T> struct get_big_int_max_size<big_int<N>, T> : public std::integral_constant<std::size_t, N> {};
    template <std::size_t N, std::integral T> struct get_big_int_max_size<T, big_int<N>> : public std::integral_constant<std::size_t, N> {};

    template <std::size_t N>
    constexpr std::strong_ordering operator<=>(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r)
    {
        constexpr auto larger = get_big_int_max_size<big_int<N>, std::remove_const_t<std::remove_reference_t<decltype(r)>>>::value;

        // �T�C�Y���傫���ق��֌^�𓝈ꂷ��
        const auto l_arr = big_int<larger>(l);
        const auto r_arr = big_int<larger>(r);

        // ���̑傫���ق������r����
        for (auto i : std::views::iota((std::size_t)0, larger) | std::views::reverse) {
            if (l_arr[i] < r_arr[i]) return std::strong_ordering::less;
            if (l_arr[i] > r_arr[i]) return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }
    template <std::size_t N>
    constexpr std::strong_ordering operator<=>(std::integral auto l, const big_int<N>& r) { return big_int<N>{l} <=> r; }

    template <std::size_t N>
    constexpr auto operator+(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r) { return convert_max_size_type_value(l, r) += r; }

    template <std::size_t N>
    constexpr auto operator-(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r) { return convert_max_size_type_value(l, r) -= r; }

    template <std::size_t N>
    constexpr auto operator*(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r) { return convert_max_size_type_value(l, r) *= r; }

    template <std::size_t N>
    constexpr auto operator/(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r) { return convert_max_size_type_value(l, r) /= r; }

    template <std::size_t N>
    constexpr auto operator%(const big_int<N>& l, const std::convertible_to<big_int<N>> auto& r) { return convert_max_size_type_value(l, r) %= r; }

    template <std::size_t N1>
    constexpr auto operator<<(const big_int<N1>& l, std::unsigned_integral auto r) { return big_int<N1>(l) <<= r; }

    template <std::size_t N1>
    constexpr auto operator>>(const big_int<N1>& l, std::unsigned_integral auto r) { return big_int<N1>(l) >>= r; }
}

#endif // TUUTILCPP_INCLUDE_GUARD_UTILITY_BIG_INT_HPP
