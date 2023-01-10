///----------------------------------
/// @file mpl/value_constant.hpp 
/// @brief 非型テンプレートパラメータを型テンプレートパラメータに持ち上げる
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP

namespace tuutil::mpl
{
    /**
     * @class
     * @brief 特性クラスのテンプレート。値を型へ持ち上げながら、is_sameの結果は型と値がともに一致した時のみ真となる
     * @tparam V コンパイル時評価可能な任意型の値
    */
    template <auto V>
    struct value_constant : public std::type_identity<decltype(V)>
    {
        static constexpr auto value = V;
    };

    /**
     * @fn
     * @brief テンプレート引数で指定した値をそのまま返す
     * @tparam V コンパイル時評価可能な任意型の値
    */
    template <auto V> constexpr auto value_constant_v = value_constant<V>::value;

    /**
     * @fn
     * @brief decltypeと一緒
     * @tparam V コンパイル時評価可能な任意型の値
    */
    template <auto V> using value_constant_t = value_constant<V>::type;

    /**
     * @fn
     * @brief value_constantかどうか判定
    */
    template <class T> struct is_value_constant : public std::false_type {};
    template <auto V> struct is_value_constant<value_constant<V>> : public std::true_type {};

    /**
     * @fn
     * @brief value_constantかどうか判定
    */
    template <class T> constexpr auto is_value_constant_v = is_value_constant<T>::value;

    /**
     * @fn
     * @brief パラメータパック未指定のテンプレートクラスを保持する
    */
    template <template <auto...> class List> struct liftv {};

    /**
     * @fn
     * @brief liftedv型か判定
    */
    template <class T> struct is_liftedv : public std::false_type {};
    template <template <auto...> class List> struct is_liftedv<liftv<List>> : public std::true_type {};

    /**
     * @fn
     * @brief liftedv型か判定
    */
    template <class T> constexpr auto is_liftedv_v = is_liftedv<T>::value;

    /**
     * @fn
     * @brief liftした非型テンプレートに非型テンプレートパラメータを適用する
    */
    template <class LiftedList, auto... Parameters> struct applyv;
    template <template <auto...> class List, auto... Parameters>
    struct applyv<liftv<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

    /**
     * @fn
     * @brief liftした非型テンプレートに非型テンプレートパラメータを適用する
    */
    template <class LiftedList, auto... Parameters> using applyv_t = applyv<LiftedList, Parameters...>::type;

    /**
     * @fn
     * @brief 値リストの各要素をvalue_constantで型に持ち上げたリストを返す
    */
    template <class ValueList> struct pack_type;
    template <template <auto...> class List, auto... Parameters>
    struct pack_type<List<Parameters...>>
        : public std::type_identity<type_list<liftv<List>, value_constant<Parameters>...>>
    {};

    /**
     * @fn
     * @brief 値リストの各要素をvalue_constantで型に持ち上げたリストを返す
    */
    template <class ValueList> using pack_type_t = pack_type<ValueList>::type;

    /**
     * @fn
     * @brief pack_typeされた値をもとに戻す
    */
    template <class TypeList> struct unpack_type;
    template <template <class...> class List, class Head, class... Parameters>
    requires (is_liftedv_v<Head> && (is_value_constant_v<Parameters> && ...))
    struct unpack_type<List<Head, Parameters...>> : public applyv<Head, Parameters::value...> {};

    /**
     * @fn
     * @brief pack_typeされた値をもとに戻す
    */
    template <class TypeList> using unpack_type_t = unpack_type<TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP
