///----------------------------------
/// @file mpl/find_if.hpp 
/// @brief メタ関数による型パラメータパックの抽出
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 内部実装
    */
    template <MetaCallable Pred, class... TypeList> struct extract_if_impl;
    // ここまで条件を満たす型がなかった場合
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters>
    struct extract_if_impl<Pred, List<Head, Parameters...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        extract_if_impl<Pred, List<Parameters...>, List<Head>>,
        extract_if_impl<Pred, List<Parameters...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head>
    struct extract_if_impl<Pred, List<Head>> : public std::conditional<
        apply_v<Pred, Head>,
        List<Head>,
        ignore_type
    > {};
    // 既に幾つか条件を満たす型が見つかっている
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters, class... Parameters2>
    struct extract_if_impl<Pred, List<Head, Parameters...>, List<Parameters2...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        extract_if_impl<Pred, List<Parameters...>, List<Parameters2..., Head>>,
        extract_if_impl<Pred, List<Parameters...>, List<Parameters2...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters2>
    struct extract_if_impl<Pred, List<Head>, List<Parameters2...>> : public std::conditional<
        apply_v<Pred, Head>,
        List<Parameters2..., Head>,
        List<Parameters2...>
    > {};

    /**
     * @fn
     * @brief メタ関数による判定が真である最初の型を取得
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> struct extract_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct extract_if<Pred, List<Parameters...>> : public extract_if_impl<Pred, List<Parameters...>> {};

    /**
     * @fn
     * @brief メタ関数による判定が真である最初の型を取得
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> using extract_if_t = extract_if<Pred, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
