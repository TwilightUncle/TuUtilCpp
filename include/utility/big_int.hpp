///----------------------------------
/// @file big_int.hpp
/// @brief 64 × Sizeビットの整数
///----------------------------------
#pragma once

namespace tudb
{
    template <std::size_t Size = 2>
    requires (Size > 1)
    struct big_int : public carry_over_container<unsigned long long, Size>
    {
        using value_type = unsigned long long;

        // 実質実装していない
        bool sign = true;

        constexpr big_int() : carry_over_container<unsigned long long, Size>{} {}

        constexpr big_int(std::integral auto i) : big_int()
        {
            this->at(0) = (i < 0) ? -i : i;
            this->sign = (i >= 0);
        }

        template <std::unsigned_integral... Types>
        requires (sizeof...(Types) == Size)
        constexpr big_int(Types... args)
            : carry_over_container<unsigned long long, Size>{(value_type)args...}
        {}

        template <std::size_t N>
        constexpr big_int(const big_int<N>& v) : big_int()
        {
            for (std::size_t i = 0; i < (std::min)(Size, N); i++) this->at(i) = v[i];
        }

        static constexpr auto plus(value_type l_v, value_type r_v)
        {
            const auto is_carried = (std::numeric_limits<value_type>::max)() - l_v < r_v;
            return std::pair{
                std::array{l_v + r_v, (value_type)is_carried},
                is_carried
            };
        }

        static constexpr auto minus(value_type l_v, value_type r_v)
        {
            const auto is_carried = (value_type)(l_v < r_v);
            const auto low = (std::numeric_limits<value_type>::max)() * is_carried - r_v + l_v + 1 * is_carried;
            return std::pair{
                std::array{(value_type)low, is_carried},
                (bool)is_carried
            };
        }

        static constexpr auto mul(value_type l_v, value_type r_v)
        {
            constexpr auto harf_digits = std::numeric_limits<value_type>::digits / 2;
            constexpr auto harf_mask = ~0ull >> harf_digits;

            // 引数をそれぞれ最大ビット数の半分で分割
            const auto upper_l = l_v >> harf_digits;
            const auto lowwer_l = l_v & harf_mask;
            const auto upper_r = r_v >> harf_digits;
            const auto lowwer_r = r_v & harf_mask;

            // たすき掛け(足し算する際、桁上がりを考慮する必要がある部分はplusで)
            const auto lowwer = lowwer_l * lowwer_r;
            const auto [middle, _dust1] = plus(lowwer_l * upper_r, upper_l * lowwer_r);
            const auto upper = upper_l * upper_r;

            // たすき掛けの結果を合計(ビット演算子と加算・減算ではビット演算子のほうが優先度が低いので注意)
            const auto [lowwer_result, _dust2] = plus(lowwer, middle[0] << harf_digits);
            const auto upper_result = upper
                + (middle[0] >> harf_digits)
                + (middle[1] << harf_digits)
                + lowwer_result[1];

            return std::pair{std::array{lowwer_result[0], upper_result}, upper_result > 0};
        }

        static constexpr auto lshift(value_type l_v, value_type r_v)
        {
            const auto high_rshfts = std::numeric_limits<value_type>::digits - r_v;
            const auto upper = l_v >> high_rshfts;
            return std::pair{std::array{l_v << r_v, upper}, upper > 0};
        }

        constexpr big_int& operator++() { return this->set_with_carry_up<big_int>(plus, 1); }

        constexpr big_int operator++(int)
        {
            big_int result{*this};
            ++(*this);
            return result;
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

        template <std::size_t N>
        constexpr big_int& operator+=(const big_int<N>& v) { return this->set_with_carry_up<big_int>(plus, v); }

        template <std::size_t N>
        constexpr big_int& operator-=(const big_int<N>& v) { return this->set_with_carry_up<big_int>(minus, v); }

        template <std::size_t N>
        constexpr big_int& operator*=(const big_int<N>& v) { return this->set_with_carry_up_all<big_int>(mul, v, plus); }

        constexpr big_int& operator<<=(std::unsigned_integral auto v)
        {
            constexpr auto value_type_max_digits = std::numeric_limits<value_type>::digits;
            // シフトで桁をまたぐ数
            const auto carry_count = v / value_type_max_digits;
            // 実際に行うシフト数
            const auto shift_value = v % value_type_max_digits;

            return this->set_with_carry_up_all<big_int>(lshift, shift_value, plus, carry_count);
        }
    };

    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const big_int<N1>& l, const big_int<N2>& r)
    {
        constexpr auto larger = (std::max)(N1, N2);

        // サイズが大きいほうへ型を統一する
        const auto l_arr = big_int<larger>(l);
        const auto r_arr = big_int<larger>(r);

        // 桁の大きいほうから比較する
        for (auto i : std::views::iota((std::size_t)0, larger) | std::views::reverse) {
            if (l_arr[i] < r_arr[i]) return std::strong_ordering::less;
            if (l_arr[i] > r_arr[i]) return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const big_int<N1>& l, const big_int<N2>r) { return big_int<(std::max)(N1, N2)>(l) += r; }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator-(const big_int<N1>& l, const big_int<N2>r) { return big_int<(std::max)(N1, N2)>(l) -= r; }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator*(const big_int<N1>& l, const big_int<N2>r) { return big_int<(std::max)(N1, N2)>(l) *= r; }

    template <std::size_t N1>
    constexpr auto operator<<(const big_int<N1>& l, std::unsigned_integral auto r) { return big_int<N1>(l) <<= r; }
}
