///----------------------------------
/// @file carry_over.hpp
/// @brief 二項の引数を受け取る処理に対して繰り上げ処理を適用できるようなコンテナ
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP
#define TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP


namespace tudb
{
    /**
     * @brief 二引数を渡して実行した結果、次のフォーマットで結果が返る -> tuple{(T)桁上がりしていない部分, (T)桁上がりした部分, (bool)桁上がりしたかどうか}
    */
    template <class F, class T>
    concept CarryOverCallable = std::is_invocable_r_v<std::tuple<T, T, bool>, F, T, T>;

    template <class T, std::size_t Size>
    struct carry_over_container : std::array<T, Size>
    {
        /**
         * @fn
         * @brief runメンバの戻り値から、継承先での型キャストがめんどくさそうなので用意
        */
        constexpr void set(const carry_over_container& v) { (*this) = v; }

        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg, const int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 同じ型が渡されたとき
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up(
            CarryOverCallable<T> auto f,
            const carry_over_container<Type, N>& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = (*this);
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.with_carry_up(f, arg[i], carry_over_process, i + offset);
            return result;
        }

        /**
         * @fn
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と同じ場合 ※足し算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param offset 計算対象のインデックスを指定
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        constexpr carry_over_container with_carry_up(CarryOverCallable<T> auto f, const T& arg, unsigned int offset = 0) const
        {
            return with_carry_up(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 桁上がりの計算をbufに適用する(桁上がりの計算が、最初と異なる場合 ※掛け算のひっ算みたいな)
         * @param f 目的の処理
         * @param arg 引数
         * @param carry_over_process 桁上がりに対して実施される処理
         * @param offset 計算対象のインデックスを指定
         * @param offset 代入先のインデックスにずれがあるとき指定
        */
        constexpr carry_over_container with_carry_up(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = (*this);
            if (offset < Size && offset >= 0) {
                auto [lower, upper, is_carry] = f(this->at(offset), arg);
                result[offset] = lower;

                for (unsigned int i = offset + 1; i < Size && is_carry; i++)
                    std::tie(result[i], upper, is_carry) = carry_over_process(this->at(i), upper);
            }
            return result;
        }

        /**
         * @fn
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const carry_over_container<Type, N>& arg) const
        {
            return with_carry_up_all(f, arg, f);
        }

        /**
         * @fn
         * @brief 自身と同じ型のオブジェクトによるたすき掛け
        */
        template <std::size_t N, std::convertible_to<T> Type>
        constexpr carry_over_container with_carry_up_all(
            CarryOverCallable<T> auto f,
            const carry_over_container<Type, N>& arg,
            CarryOverCallable<T> auto carry_over_process
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < (std::min)(Size, N); i++)
                result = result.with_carry_up(
                    carry_over_process,
                    with_carry_up_all(f, arg[i], carry_over_process, i)
                );
            return result;
        }

        /**
         * @fn
         * @brief 引数をすべての要素に適用する
        */
        constexpr carry_over_container with_carry_up_all(CarryOverCallable<T> auto f, const T& arg, const int offset = 0) const
        {
            return with_carry_up_all(f, arg, f, offset);
        }

        /**
         * @fn
         * @brief 引数をすべての要素に適用する
        */
        constexpr carry_over_container with_carry_up_all(
            CarryOverCallable<T> auto f,
            const T& arg,
            CarryOverCallable<T> auto carry_over_process,
            const int offset = 0
        ) const {
            auto result = carry_over_container{};
            for (unsigned int i = 0; i < Size; i++) {
                const auto [lower, upper, is_carry] = f(this->at(i), arg);
                result = result.with_carry_up(
                    carry_over_process,
                    carry_over_container<T, 2>{lower, upper},
                    i + offset
                );
            }
            return result;
        }

        template <class Derived, class... ArgTypes>
        requires std::is_base_of_v<carry_over_container, Derived>
        constexpr Derived& set_with_carry_up(const ArgTypes&... args)
        {
            this->set(this->with_carry_up(args...));
            return static_cast<Derived&>(*this);
        }

        template <class Derived, class... ArgTypes>
        requires std::is_base_of_v<carry_over_container, Derived>
        constexpr Derived& set_with_carry_up_all(const ArgTypes&... args)
        {
            this->set(this->with_carry_up_all(args...));
            return static_cast<Derived&>(*this);
        }
    };

    /**
     * @fn
     * @brief 整数型を格納しているものに対して、上位要素の0を未使用とみなし、使用中の要素数を数える。全て0の場合は1
    */
    template <std::unsigned_integral T, std::size_t Size>
    constexpr auto count_using_size(const carry_over_container<T, Size>& v) noexcept
    {
        auto result = Size;
        while (!v[(--result)] && result > 0);
        return result + 1;
    }

    /**
     * @fn
     * @brief 整数型を格納したcarry_over_containerを情報の精度を失わないようにToBufTのサイズの整数型のcarry_over_containerへ変換する
    */
    template <std::unsigned_integral ToBufT, std::unsigned_integral FromBufT, std::size_t Size>
    constexpr auto convert_diff_size_buffer(const carry_over_container<FromBufT, Size>& from) noexcept
    {
        constexpr auto from_buffer_digits = std::numeric_limits<FromBufT>::digits;
        constexpr auto from_all_digits = from_buffer_digits * Size;
        constexpr auto to_buffer_digits = std::numeric_limits<ToBufT>::digits;
        constexpr auto to_buffer_size = from_all_digits / to_buffer_digits + (bool)(from_all_digits % to_buffer_digits);
        using larger_type = std::conditional_t<(from_buffer_digits > to_buffer_digits), FromBufT, ToBufT>;

        auto to =  carry_over_container<ToBufT, to_buffer_size>{};
        for (auto dig = 0; dig < from_all_digits; dig += (std::min)(from_buffer_digits, to_buffer_digits)) {
            const auto to_i = dig / to_buffer_digits;
            const auto from_i = dig / from_buffer_digits;
            const auto diff_digits = (int)(to_i * to_buffer_digits) - (int)(from_i * from_buffer_digits);
            // シフト前に大きいほうのサイズの型に統一(小さいままだと、シフト数が桁に対して多すぎ、コンパイルできない場合がある)
            to[dig / to_buffer_digits] |= ((larger_type)from[dig / from_buffer_digits] >> (std::max)(0, diff_digits)) << (std::max)(0, -diff_digits);
        }
        return to;
    }
    template <std::unsigned_integral ToBufT, std::unsigned_integral FromBufT>
    constexpr auto convert_diff_size_buffer(const FromBufT& from) noexcept
    {
        return convert_diff_size_buffer<ToBufT>(carry_over_container<FromBufT, 1>{from});
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_CARRY_OVER_HPP
