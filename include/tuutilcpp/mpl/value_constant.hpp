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
    template <auto V> using value_constant_t = typename value_constant<V>::type;

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

    template <class T>
    concept LiftedvList = is_liftedv<T>::value;

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
    template <class LiftedList, auto... Parameters> using applyv_t = typename applyv<LiftedList, Parameters...>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_VALUE_CONSTANT_HPP
