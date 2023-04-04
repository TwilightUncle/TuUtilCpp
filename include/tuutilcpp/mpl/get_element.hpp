///----------------------------------
/// @file mpl/find_if.hpp 
/// @brief メタ関数による型パラメータパックの検索
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_GET_ELEMENT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_GET_ELEMENT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 内部実装
    */
    template <std::size_t N, std::size_t Cnt, class... Parameters>
    struct get_impl;
    template <std::size_t N, std::size_t Cnt, class Head, class... Parameters>
    struct get_impl<N, Cnt, Head, Parameters...> : public get_impl<N, Cnt + 1, Parameters...> {};
    template <std::size_t N, std::size_t Cnt, class Head, class... Parameters>
    requires (N == Cnt)
    struct get_impl<N, Cnt, Head, Parameters...> : public std::type_identity<Head> {};

    /**
     * @fn
     * @brief テンプレートパラメータの指定した位置にある要素を取得
     * @tparam N 位置
     * @tparam List パラメータパックを持つ型
    */
    template <std::size_t N, class List> struct get;
    template <std::size_t N, template <class...> class List, class... Parameters>
    struct get<N, List<Parameters...>> : public get_impl<N, 0, Parameters...> {};
    template <std::size_t N, template <auto...> class List, auto... Parameters>
    struct get<N, List<Parameters...>> : public get_impl<N, 0, value_constant<Parameters>...>::type {};

    /**
     * @fn
     * @brief テンプレートパラメータの指定した位置にある要素を取得
     * @tparam N 位置
     * @tparam List パラメータパックを持つ型
    */
    template <std::size_t N, class List> using get_t = typename get<N, List>::type;

    /**
     * @fn
     * @brief テンプレートパラメータの指定した位置にある要素を取得
     * @tparam N 位置
     * @tparam List パラメータパックを持つ型
    */
    template <std::size_t N, class List> constexpr auto get_v = get<N, List>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_GET_ELEMENT_HPP
