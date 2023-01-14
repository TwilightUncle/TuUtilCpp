///----------------------------------
/// @file mpl/reverse.hpp 
/// @brief 型パラメータパックの並びを反転
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief パラメータパックの数を数える
     * @tparam T 型パラメータパックを持つ型
    */
    template <class T> struct count;
    template <template <class...> class List, class... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};
    template <template <auto...> class List, auto... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};
    template <>
    struct count<ignore_type> : public std::integral_constant<std::size_t, 0> {};

    /**
     * @fn
     * @brief パラメータパックの数を数える
     * @tparam T 型パラメータパックを持つ型
    */
    template <class T> constexpr auto count_v = count<T>::value;

    /**
     * @fn
     * @brief 条件に合致するパラメータパックの数を数える
     * @tparam Pred 判定用メタ関数
     * @tparam T 型パラメータパックを持つ型
    */
    template <MetaCallable Pred, class T> using count_if = relay<
        T,
        type_list<
            bind<quote<extract_if>, Pred>,
            quote<count>
        >
    >;

    template <MetaCallable Pred, class T> constexpr auto count_if_v = count_if<Pred, T>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP
