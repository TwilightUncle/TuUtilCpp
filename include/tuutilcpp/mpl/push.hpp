///----------------------------------
/// @file mpl/push_pop.hpp 
/// @brief メタ関数による型パラメータパックの一括適用
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief パラメータパックの最後尾に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <class TypeList, class T> struct push_back;
    template <template <class...> class List, class... Parameters, class T>
    struct push_back<List<Parameters...>, T> : public std::type_identity<List<Parameters..., T>> {};

    /**
     * @fn
     * @brief パラメータパックの最後尾に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <class TypeList, class T> using push_back_t = typename push_back<TypeList, T>::type;

    /**
     * @fn
     * @brief パラメータパックの先頭に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <class TypeList, class T> struct push_front;
    template <template <class...> class List, class... Parameters, class T>
    struct push_front<List<Parameters...>, T> : public std::type_identity<List<T, Parameters...>> {};

    /**
     * @fn
     * @brief パラメータパックの先頭に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <class TypeList, class T> using push_front_t = typename push_front<TypeList, T>::type;

    /**
     * @fn
     * @brief 追加しようとする型が条件に合致する場合のみパラメータパックの最後尾に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <MetaCallable Pred, class TypeList, class T>
    using push_back_if = std::conditional<apply_v<Pred, T>, push_back_t<TypeList, T>, TypeList>;

    /**
     * @fn
     * @brief 追加しようとする型が条件に合致する場合のみパラメータパックの最後尾に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <MetaCallable Pred, class TypeList, class T> using push_back_if_t = typename push_back_if<Pred, TypeList, T>::type;

    /**
     * @fn
     * @brief 追加しようとする型が条件に合致する場合のみパラメータパックの先頭に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <MetaCallable Pred, class TypeList, class T>
    using push_front_if = std::conditional<apply_v<Pred, T>, push_front_t<TypeList, T>, TypeList>;

    /**
     * @fn
     * @brief 追加しようとする型が条件に合致する場合のみパラメータパックの先頭に型を追加する
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam T 追加する型
    */
    template <MetaCallable Pred, class TypeList, class T> using push_front_if_t = typename push_front_if<Pred, TypeList, T>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_PUSH_HPP
