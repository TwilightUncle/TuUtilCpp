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
     * @tparam List パラメータパックを持つ型
    */
    template <MetaCallable Pred, class List> struct extract_if;
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
    template <MetaCallable Pred, template <auto...> class List, auto... Parameters>
    struct extract_if<Pred, List<Parameters...>>
        : public behave_as_type_list_arg<bind<quote<extract_if>, Pred>, List<Parameters...>>
    {};

    /**
     * @fn
     * @brief メタ関数による判定が真である型を抽出
     * @tparam Pred 判定用メタ関数
     * @tparam List パラメータパックを持つ型
    */
    template <MetaCallable Pred, class List> using extract_if_t = extract_if<Pred, List>::type;

    /**
     * @fn
     * @brief メタ関数による判定が真である型を除外(extract_ifの判定式の結果を反転させている)
     * @tparam Pred 判定用メタ関数
     * @tparam List パラメータパックを持つ型
    */
    template <MetaCallable Pred, class List> struct remove_if
        : public extract_if<bind<quote<flip>, quote<relay>, type_list<Pred, quote<std::negation>>>, List>
    {};

    /**
     * @fn
     * @brief メタ関数による判定が真である型を除外
     * @tparam Pred 判定用メタ関数
     * @tparam List パラメータパックを持つ型
    */
    template <MetaCallable Pred, class List> using remove_if_t = remove_if<Pred, List>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
