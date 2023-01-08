///----------------------------------
/// @file mpl/parameter_pack.hpp 
/// @brief 型パラメータパックの操作や判定
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief テンプレートクラスを型パラメータとして利用できるようにする
     * @tparam List パラメータパック未指定のテンプレート型
    */
    template <template <class...> class List> struct lift {};

    /**
     * @fn
     * @brief liftで包まれたテンプレート型か判定
     * @tparam T 検査対象
    */
    template <class T> struct is_lifted : public std::false_type {};
    template <template <class...> class List>
    struct is_lifted<lift<List>> : public std::true_type {};

    template <class T>
    concept LiftedList = is_lifted<T>::value;

    /**
     * @fn
     * @brief liftで包まれたテンプレートクラスにパラメータパックを適用する
    */
    template <template <class...> class List, class... Parameters>
    struct apply<lift<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

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

    /**
     * @fn
     * @brief 型のパラメータパックの並びを左方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct rotatel;
    template <template <class...> class List, class Head, class... Parameters>
    struct rotatel<List<Head, Parameters...>> : public std::type_identity<List<Parameters..., Head>> {};

    /**
     * @fn
     * @brief 型のパラメータパックの並びを左方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using rotatel_t = rotatel<TypeList>::type;

    /**
     * @fn
     * @brief 型のパラメータパックの並びを右方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> struct rotater;
    template <template <class...> class List, class... Parameters>
    struct rotater<List<Parameters...>> : public reverse<rotatel_t<reverse_t<List<Parameters...>>>> {};

    /**
     * @fn
     * @brief 型のパラメータパックの並びを右方向に一つずらし、先頭の型を最後尾に移す
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class TypeList> using rotater_t = rotater<TypeList>::type;

    /**
     * @fn
     * @brief 型のパラメータパックを持つか判定
     * @tparam T 対象
    */
    template <class T> struct has_type_parameters : public std::false_type {};
    template <template <class...> class List, class... Parameters>
    struct has_type_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief 型のパラメータパックを持つか判定
     * @tparam T 検査対象
    */
    template <class T> constexpr auto has_type_parameters_v = has_type_parameters<T>::value;

    /**
     * @fn
     * @brief パラメータパックの内容をDestへコピー
     * @tparam Src コピー元の型パラメータパックを持つ型
     * @tparam Dest コピー対象のテンプレートクラス
    */
    template <class Src, LiftedList Dest> struct copy;
    template <template <class...> class Src, class Dest, class... Parameters>
    struct copy<Src<Parameters...>, Dest> : public apply<Dest, Parameters...> {};

    /**
     * @fn
     * @brief パラメータパックの内容をDestへコピー
     * @tparam Src コピー元の型パラメータパックを持つ型
     * @tparam Dest コピー対象のテンプレートクラス
    */
    template <class Src, LiftedList Dest> using copy_t = copy<Src, Dest>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
