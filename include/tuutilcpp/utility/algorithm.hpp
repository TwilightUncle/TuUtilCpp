///----------------------------------
/// @file algorithm.hpp 
/// @brief 汎用メタ関数定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP
#define TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP

namespace tuutil::utility
{
    /**
     * @fn
     * @brief テンプレート引数で渡した整数値の桁数を取得する。第二引数は任意の進数を指定する
    */
    template <auto V, std::size_t Hex = 10>
    requires (Hex >= 2 && (std::is_integral_v<decltype(V)> || is_big_int_v<decltype(V)>))
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP
