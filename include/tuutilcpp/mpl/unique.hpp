///----------------------------------
/// @file mpl/unique.hpp 
/// @brief 型パラメータパックを一意にする
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 内部実装
    */
    template <class TypeList1, class TypeList2> struct unique_impl;
    template <template <class...> class List, class Dest, class Head, class... Parameters>
    struct unique_impl<List<Head, Parameters...>, Dest> : public unique_impl<
        remove_if_t<bind<quote<std::is_same>, Head>, List<Parameters...>>,
        push_back_t<Dest, Head>
    > {};
    template <template <class...> class List, class Dest, class Head>
    struct unique_impl<List<Head>, Dest> : public unique_impl<ignore_type, push_back_t<Dest, Head>> {};
    template <class Dest>
    struct unique_impl<ignore_type, Dest> : public std::type_identity<Dest> {};

    /**
     * @fn
     * @brief パラメータパックの重複を排除する
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct unique;
    template <template <class...> class List, class... Parameters>
    struct unique<List<Parameters...>> : public unique_impl<List<Parameters...>, lift<List>> {};

    /**
     * @fn
     * @brief パラメータパックの重複を排除する
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using unique_t = unique<TypeList>::type;

    /**
     * @fn
     * @brief パラメータパックが一意か判定
     * @tparam TypeList 型パラメータパックをもつ型
    */
    template <class TypeList>
    struct is_unique : public std::is_same<TypeList, unique_t<TypeList>> {};

    /**
     * @fn
     * @brief パラメータパックが一意か判定
     * @tparam TypeList 型パラメータパックをもつ型
    */
    template <class TypeList> constexpr auto is_unique_v = is_unique<TypeList>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP
