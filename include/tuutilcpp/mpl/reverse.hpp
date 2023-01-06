///----------------------------------
/// @file mpl/reverse.hpp 
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
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
