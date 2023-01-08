///----------------------------------
/// @file mpl/extract_if.hpp 
/// @brief メタ関数による型パラメータパックの抽出
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief メタ関数による判定が真である型を抽出
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> struct extract_if;
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters>
    struct extract_if<Pred, List<Head, Parameters...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        foldl<bind<quote<push_back_if>, Pred>, List<Head>, List<Parameters...>>,
        extract_if<Pred, List<Parameters...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head>
    struct extract_if<Pred, List<Head>>
        : public std::conditional<apply_v<Pred, Head>, List<Head>, ignore_type>
    {};

    /**
     * @fn
     * @brief メタ関数による判定が真である型を抽出
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> using extract_if_t = extract_if<Pred, TypeList>::type;

    /**
     * @fn
     * @brief メタ関数による判定が真である型を除外(extract_ifの判定式の結果を反転させている)
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> struct remove_if
        : public extract_if<bind<quote<flip>, quote<relay>, type_list<Pred, quote<std::negation>>>, TypeList>
    {};

    /**
     * @fn
     * @brief メタ関数による判定が真である型を除外
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> using remove_if_t = remove_if<Pred, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
