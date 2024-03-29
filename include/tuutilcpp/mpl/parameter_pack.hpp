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
     * @brief パラメータパックを持つ型のガワの部分を取得する
     * @return lift or liftvで包まれたテンプレート型
    */
    template <class T> struct get_empty_list;
    template <template <class...> class List, class... Parameters>
    struct get_empty_list<List<Parameters...>> : public std::type_identity<lift<List>> {};
    template <template <auto...> class List, auto... Parameters>
    struct get_empty_list<List<Parameters...>> : public std::type_identity<liftv<List>> {};

    /**
     * @fn
     * @brief パラメータパックを持つ型のガワの部分を取得する
     * @return lift or liftvで包まれたテンプレート型
    */
    template <class T> using get_empty_list_t = get_empty_list<T>::type;

    /**
     * @fn
     * @brief liftで包まれたテンプレートクラスにパラメータパックを適用する
    */
    template <template <class...> class List, class... Parameters>
    struct apply<lift<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

    /**
     * @fn
     * @brief パラメータパックの並びを反転させる
     * @tparam List 型のパラメータパックを持つ型
    */
    template <class List> struct reverse;
    template <template <class...> class List, class Head, class... Parameters>
    struct reverse<List<Head, Parameters...>> : public foldl<quote<push_front>, List<Head>, List<Parameters...>> {};

    /**
     * @fn
     * @brief 型のパラメータパックの並びを反転させる
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <class List> using reverse_t = reverse<List>::type;

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
     * @brief パラメータパックの型が全て同じとき真
     * @tparam T 検査対象の型パラメータパックを持つ型
    */
    template <class T> struct is_same_types;
    template <template <class...> class List, class Head, class... Parameters>
    struct is_same_types<List<Head, Parameters...>> : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};
    // 非型テンプレートパラメータの型部分のみの比較(具体的な値については関知しない)
    template <template <auto...> class List, auto... Parameters>
    struct is_same_types<List<Parameters...>> : public is_same_types<type_list<decltype(Parameters)...>> {};
    template <> struct is_same_types<ignore_type> : public std::true_type {};

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

    /**
     * @fn
     * @brief 先頭要素を取得する
     * @tparam パラメータパックを持つ型
    */
    template <class List> struct get_front;
    template <template <class...> class List, class Head, class... Parameters>
    struct get_front<List<Head, Parameters...>> : public std::type_identity<Head> {};
    template <template <auto...> class List, auto Head, auto... Parameters>
    struct get_front<List<Head, Parameters...>> : public value_constant<Head> {};

    template <class List> using get_front_t = get_front<List>::type;
    template <class ValueList> constexpr auto get_front_v = get_front<ValueList>::value;

    /**
     * @fn
     * @brief 最後尾要素を取得する
     * @tparam パラメータパックを持つ型
    */
    template <class List> using get_back = get_front<reverse_t<List>>;

    template <class List> using get_back_t = get_back<List>::type;
    template <class ValueList> constexpr auto get_back_v = get_back<ValueList>::value;

    /**
     * @fn
     * @brief 1つ以上のパラメータパックの内容を結合する
    */
    template <class... Lists>
    requires (sizeof...(Lists) > 0)
    struct concat_list;
    template <class List>
    requires(has_type_parameters_v<List> || has_value_parameters_v<List>)
    struct concat_list<List> : std::type_identity<List> {};
    // 二つのtype_listを結合
    template <template <class...> class List, class... Types1, class... Types2>
    struct concat_list<List<Types1...>, List<Types2...>> : std::type_identity<List<Types1..., Types2...>> {};
    // 任意数のtype_listを結合する
    template <class Head, class... Lists>
    requires (has_type_parameters_v<Lists> && ...)
    struct concat_list<Head, Lists...> : public foldl<quote<concat_list>, Head, type_list<Lists...>> {};

    template <class... Lists> using concat_list_t = concat_list<Lists...>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
