///----------------------------------
/// @file algorithm.hpp 
/// @brief �ėp���^�֐���`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP
#define TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP

namespace tuutil::utility
{
    /**
     * @fn
     * @brief �e���v���[�g�����œn���������l�̌������擾����B�������͔C�ӂ̐i�����w�肷��
    */
    template <auto V, std::size_t Hex = 10>
    requires (Hex >= 2 && (std::integral<decltype(V)> || std::convertible_to<decltype(V), big_int<2>>))
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_UTILITY_ALGORITHM_HPP
