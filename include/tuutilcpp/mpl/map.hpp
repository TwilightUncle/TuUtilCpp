///----------------------------------
/// @file mpl/fold.hpp 
/// @brief メタ関数による型パラメータパックの一括適用
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class TypeList> struct map;
    template <MetaCallable F, template <class...> class List, class... Parameters>
    struct map<F, List<Parameters...>> : public std::type_identity<List<apply_t<F, Parameters>...>> {};

    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class TypeList> using map_t = map<F, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
