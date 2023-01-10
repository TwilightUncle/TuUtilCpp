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

    template <template <class...> class List, class T>
    struct push_back<lift<List>, T> : public std::type_identity<List<T>> {};

    template <template <class...> class List, class T>
    struct push_front<lift<List>, T> : public std::type_identity<List<T>> {};

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
     * @brief 非型のパラメータパックを持つか判定
     * @tparam T 対象
    */
    template <class T> struct has_value_parameters : public std::false_type {};
    template <template <auto...> class List, auto... Parameters>
    struct has_value_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief 型のパラメータパックを持つか判定
     * @tparam T 検査対象
    */
    template <class T> constexpr auto has_type_parameters_v = has_type_parameters<T>::value;

    /**
     * @fn
     * @brief 非型のパラメータパックを持つか判定
     * @tparam T 対象
    */
    template <class T> constexpr auto has_value_parameters_v = has_value_parameters<T>::value;

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

    /**
     * @fn
     * @brief パラメータパックの数を数える
     * @tparam T 型パラメータパックを持つ型
    */
    template <class T> constexpr auto count_v = count<T>::value;

    /**
     * @fn
     * @brief パラメータパックの型が全て同じとき真
     * @tparam T 検査対象の型パラメータパックを持つ型
    */
    template <class T> struct is_same_types;
    template <template <class...> class List, class Head, class... Parameters>
    struct is_same_types<List<Head, Parameters...>> : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};
    // 非型テンプレートパラメータの型部分のみの比較(具体的な値については関知しない)
    template <template <auto...> class List, auto... Parameters>
    struct is_same_types<List<Parameters...>> : public is_same_types<type_list<decltype(Parameters)...>> {};

    /**
     * @fn
     * @brief パラメータパックの型が全て同じとき真
     * @tparam T 検査対象の型パラメータパックを持つ型
    */
    template <class T> constexpr auto is_same_types_v = is_same_types<T>::value;

    /**
     * @fn
     * @brief 型TがListのパラメータパックに含まれているとき真
     * @tparam T 含まれているか確認したい型
     * @tparam List パラメータパックを持つ型
    */
    template <class T, class List> struct contains;
    template <class T, template <class...> class List, class... Parameters>
    struct contains<T, List<Parameters...>> : public std::bool_constant<(std::is_same_v<T, Parameters> || ...)> {};

    /**
     * @fn
     * @brief 型TがListのパラメータパックに含まれているとき真
     * @tparam T 含まれているか確認したい型
     * @tparam List パラメータパックを持つ型
    */
    template <class T, class List> constexpr auto contains_v = contains<T, List>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
