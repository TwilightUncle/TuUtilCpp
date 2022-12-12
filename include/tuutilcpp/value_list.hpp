///----------------------------------
/// @file tuutilcpp/value_list.hpp
/// @brief テンプレート引数操作系のヘルパ群
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_VALUE_LIST_HPP
#define TUUTILCPP_INCLUDE_GUARD_VALUE_LIST_HPP

namespace tudb
{
    /**
     * @class
     * @brief 値のコンテナ。パラメータパックにより操作
    */
    template <auto... Values> struct value_list
    {
        static constexpr auto size = sizeof...(Values);
    };

    /**
     * @fn
     * @brief value_listか判定
    */
    template <class T> struct is_value_list : public std::false_type {};
    template <auto... Values> struct is_value_list<value_list<Values...>> : public std::true_type {};
    template <class T> constexpr auto is_value_list_v = is_value_list<T>::value;

    /**
     * @fn
     * @brief 本ライブラリ内で、テンプレートパラメータを抜き出すときの識別子
    */
    template <class T> struct pass_values;
    template <template <auto...> class T, auto... Parameters> struct pass_values<T<Parameters...>> {};

    /**
     * @fn
     * @brief Srcのパラメータパックの内容がDestのテンプレート引数として展開される
    */
    template <class Src, template <auto...> class Dest> struct copy_values;
    template <template <auto...> class Src, template <auto...> class Dest, auto... Parameters>
    struct copy_values<Src<Parameters...>, Dest> : public std::type_identity<Dest<Parameters...>> {};

    template <class Src, template <auto...> class Dest> using copy_values_t = copy_values<Src, Dest>::type;

    /**
     * @fn
     * @brief 型のパラメータパックを持つか判定
    */
    template <class T> struct has_value_parameters : public std::false_type {};
    template <template <auto...> class T, auto... Parameters>
    struct has_value_parameters<T<Parameters...>> : public std::true_type {};

    template <class T> constexpr auto has_value_parameters_v = has_value_parameters<T>::value;

    //-----------------------------------------------------
    // 以下、algorithmのパラメータパック抜き取り版
    //-----------------------------------------------------

    template <template <auto...> class Container, auto... Parameters>
    struct get_first_type<pass_values<Container<Parameters...>>> : public get_first_type<decltype(Parameters)...> {};

    template <template <auto...> class Container, auto... Parameters>
    struct count_parameters<pass_values<Container<Parameters...>>> : public count_value_parameters<Parameters...> {};

    template <template <auto...> class Container, auto... Parameters>
    struct is_unique<pass_values<Container<Parameters...>>> : public is_unique_values<Parameters...> {};

    template <template <auto...> class Container, auto... Parameters>
    struct is_same_types<pass_values<Container<Parameters...>>> : public is_same_value_types<Parameters...> {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_VALUE_LIST_HPP
