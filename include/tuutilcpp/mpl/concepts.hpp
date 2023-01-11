///----------------------------------
/// @file mpl/concepts.hpp
/// @brief 汎用の制約定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP

namespace tuutil::mpl
{
    /**
     * @brief 列挙型であることを強制するコンセプト(ほんとはscoped_enumを判定したいがc++23から)
    */
    template <class T> concept Enumeration = std::is_enum_v<T>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_CONCEPTS_HPP
