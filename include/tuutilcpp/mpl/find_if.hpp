///----------------------------------
/// @file mpl/find_if.hpp 
/// @brief メタ関数による型パラメータパックの検索
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 内部実装
    */
    template <MetaCallable Pred, int N, class... Parameters> struct find_if_impl;
    template <MetaCallable Pred, int N, class Head, class... Parameters>
    struct find_if_impl<Pred, N, Head, Parameters...> : public std::conditional_t<
        apply_v<Pred, Head>,
        type_value_pair<Head, N>,
        find_if_impl<Pred, N + 1, Parameters...>
    > {};
    template <MetaCallable Pred, int N>
    struct find_if_impl<Pred, N> : public type_value_pair<ignore_type, int(-1)> {};

    /**
     * @fn
     * @brief メタ関数による判定が真である最初の型または位置のインデックスを取得
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> struct find_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct find_if<Pred, List<Parameters...>> : public find_if_impl<Pred, 0, Parameters...> {};

    /**
     * @fn
     * @brief メタ関数による判定が真である最初の型を取得
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> using find_if_t = typename find_if<Pred, TypeList>::type;

    /**
     * @fn
     * @brief メタ関数による判定が真である最初の型の位置を取得 
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> constexpr auto find_if_v = find_if<Pred, TypeList>::value;

    /**
     * @fn
     * @brief 指定条件の型が存在しているか判定
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> using exists_if = std::negation<std::is_same<find_if_t<Pred, TypeList>, ignore_type>>;
    
    /**
     * @fn
     * @brief 指定条件の型が存在しているか判定
     * @tparam Pred 判定用メタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, class TypeList> constexpr auto exists_if_v = exists_if<Pred, TypeList>::value;

    /**
     * @fn
     * @brief 指定の型が存在しているか判定
     * @tparam T 検索対象の型
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class T, class TypeList> using exists = exists_if<bind<quote<std::is_same>, T>, TypeList>;
    
    /**
     * @fn
     * @brief 指定の型が存在しているか判定
     * @tparam T 検索対象の型
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class T, class TypeList> constexpr auto exists_v = exists<T, TypeList>::value;

    /**
     * @fn
     * @brief 指定したTがTypeListの何番目の要素であるかを取得する。見つからない場合は-1
     * @tparam T 検索対象の型
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class T, class TypeList> using index_of = find_if<bind<quote<std::is_same>, T>, TypeList>;

    /**
     * @fn
     * @brief 指定したTがTypeListの何番目の要素であるかを取得する。見つからない場合は-1
     * @tparam T 検索対象の型
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class T, class TypeList> constexpr auto index_of_v = index_of<T, TypeList>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
