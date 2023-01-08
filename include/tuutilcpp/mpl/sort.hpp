///----------------------------------
/// @file mpl/sort.hpp 
/// @brief 型パラメータパックの並びを反転
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 型のパラメータパックの並びを反転させる
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct reverse;
    template <template <class...> class List, class Head, class... Parameters>
    struct reverse<List<Head, Parameters...>> : public foldl<quote<push_front>, List<Head>, List<Parameters...>> {}; 

    /**
     * @fn
     * @brief 型のパラメータパックの並びを反転させる
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using reverse_t = reverse<TypeList>::type;

    /**
     * @fn
     * @brief 型のパラメータパックの並びを左方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct rotatel;
    template <template <class...> class List, class Head, class... Parameters>
    struct rotatel<List<Head, Parameters...>> : public std::type_identity<List<Parameters..., Head>> {};

    /**
     * @fn
     * @brief 型のパラメータパックの並びを左方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using rotatel_t = rotatel<TypeList>::type;

    /**
     * @fn
     * @brief 型のパラメータパックの並びを右方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct rotater;
    template <template <class...> class List, class... Parameters>
    struct rotater<List<Parameters...>> : public reverse<rotatel_t<reverse_t<List<Parameters...>>>> {};

    /**
     * @fn
     * @brief 型のパラメータパックの並びを右方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using rotater_t = rotater<TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
