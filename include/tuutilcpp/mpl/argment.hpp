///----------------------------------
/// @file mpl/argment.hpp 
/// @brief メタ関数引数関連の操作
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 二引数メタ関数の引数の順番を入れ替えたうえで実行
     * @tparam F メタ関数
     * @tparam Arg2 メタ関数の二つ目に指定したい引数
     * @tparam Arg1 メタ関数の一つ目に指定したい引数
    */
    template <MetaCallable F, class Arg2, class Arg1> using flip = apply<F, Arg1, Arg2>;

    /**
     * @fn
     * @brief 二引数メタ関数の引数の順番を入れ替えたうえで実行
     * @tparam F メタ関数
     * @tparam Arg2 メタ関数の二つ目に指定したい引数
     * @tparam Arg1 メタ関数の一つ目に指定したい引数
    */
    template <MetaCallable F, class Arg2, class Arg1> using flip_t = flip<F, Arg2, Arg1>::type;

    template <MetaCallable F, class Arg2, class Arg1> constexpr auto flip_v = flip<F, Arg2, Arg1>::value;

    /**
     * @fn
     * @brief メタ関数リストへArgを食わせ、結果を逐次実行する
     * @tparam Arg 初期値
     * @tparam MetaFuncList メタ関数リスト
    */
    template <class Arg, class MetaFuncList> struct relay;
    template <class Arg, template <class...> class List, MetaCallable... Funcs>
    struct relay<Arg, List<Funcs...>> : public foldr<quote<apply>, Arg, reverse_t<List<Funcs...>>> {};

    /**
     * @fn
     * @brief メタ関数リストへArgを食わせ、結果を逐次実行する
     * @tparam Arg 初期値
     * @tparam MetaFuncList メタ関数リスト
    */
    template <class Arg, class MetaFuncList> using relay_t = relay<Arg, MetaFuncList>::type;

    /**
     * @fn
     * @brief メタ関数リストへArgを食わせ、結果を逐次実行する
     * @tparam Arg 初期値
     * @tparam MetaFuncList メタ関数リスト
    */
    template <class Arg, class MetaFuncList> constexpr auto relay_v = relay<Arg, MetaFuncList>::value;

    /**
     * @fn
     * @brief リストとして渡された型リストを引数として関数に適用する
     * @tparam F メタ関数
     * @tparam ArgList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class ArgList> struct apply_list;
    template <MetaCallable F, template <class...> class List, class... Args>
    struct apply_list<F, List<Args...>> : public apply<F, Args...> {};

    /**
     * @fn
     * @brief リストとして渡された型リストを引数として関数に適用する
     * @tparam F メタ関数
     * @tparam ArgList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class ArgList> using apply_list_t = apply_list<F, ArgList>::type;

    /**
     * @fn
     * @brief リストとして渡された型リストを引数として関数に適用する
     * @tparam F メタ関数
     * @tparam ArgList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class ArgList> constexpr auto apply_list_v = apply_list<F, ArgList>::value;

    /**
     * @fn
     * @brief 値リストの全ての要素を型として扱えるようにvalue_constantで包む
     * @tparam ValueList 非型テンプレートパラメータパックを持つ型
     * @tparam NewList value_constantで包んだ要素を受け取る、型テンプレートパラメータパックを指定可能なテンプレート型
    */
    template <class ValueList, LiftedList NewList> struct wrap_value_elements;
    template <template <auto...> class List, template <class...> class NewList, auto... ValueElements>
    struct wrap_value_elements<List<ValueElements...>, lift<NewList>>
        : public std::type_identity<NewList<value_constant<ValueElements>...>>
    {};

    /**
     * @fn
     * @brief 値リストの全ての要素を型として扱えるようにvalue_constantで包む
     * @tparam ValueList 非型テンプレートパラメータパックを持つ型
     * @tparam NewList value_constantで包んだ要素を受け取る、型テンプレートパラメータパックを指定可能なテンプレート型
    */
    template <class ValueList, LiftedList NewList> using wrap_value_elements_t = wrap_value_elements<ValueList, NewList>::type;

    /**
     * @fn
     * @brief 型リストの全ての要素を値へ変換する。全ての要素型がconstexpr valueメンバ変数を持っていないとコンパイルエラー
     * @tparam TypeList 型テンプレートパラメータパックを持つ型
     * @tparam NewList 値の要素を格納可能なテンプレート型
    */
    template <class TypeList, LiftedvList NewList> struct unwrap_value_elements;
    template <template <class...> class List, template <auto...> class NewList, class... TypeElements>
    requires (has_constexpr_value_member_v<TypeElements> && ...)
    struct unwrap_value_elements<List<TypeElements...>, liftv<NewList>>
        : public std::type_identity<NewList<TypeElements::value...>>
    {};
    template <template <auto...> class NewList>
    struct unwrap_value_elements<ignore_type, liftv<NewList>> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief 型リストの全ての要素を値へ変換する。全ての要素型がconstexpr valueメンバ変数を持っていないとコンパイルエラー
     * @tparam TypeList 型テンプレートパラメータパックを持つ型
     * @tparam NewList 値の要素を格納可能なテンプレート型
    */
    template <class TypeList, LiftedvList NewList> using unwrap_value_elements_t = unwrap_value_elements<TypeList, NewList>::type;

    /**
     * @fn
     * @brief 値リストを型リストとして扱うことによって、リスト操作用メタ関数を適用できるようにする
    */
    template <MetaCallable F, class ValueList> struct behave_as_type_list_arg;
    template <MetaCallable F, template <auto...> class List, auto... Parameters>
    struct behave_as_type_list_arg<F, List<Parameters...>> : public relay<
        List<Parameters...>,
        type_list<
            bind<quote<flip>, quote<wrap_value_elements>, lift<type_list>>,
            F,
            bind<quote<flip>, quote<unwrap_value_elements>, liftv<List>>
        >
    > {};

    // 別ファイルで宣言されているやつ

    template <template <auto...> class List, auto... Parameters>
    struct reverse<List<Parameters...>> : public behave_as_type_list_arg<quote<reverse>, List<Parameters...>> {};

    template <template <auto...> class List, auto... Parameters>
    struct rotatel<List<Parameters...>> : public behave_as_type_list_arg<quote<rotatel>, List<Parameters...>> {};

    template <template <auto...> class List, auto... Parameters>
    struct rotater<List<Parameters...>> : public behave_as_type_list_arg<quote<rotater>, List<Parameters...>> {};

    template <class Head, class... Lists>
    requires (has_value_parameters_v<Head> && (has_value_parameters_v<Lists> && ...))
    struct concat_list<Head, Lists...> : public apply<
        bind<quote<flip>, quote<unwrap_value_elements>, get_empty_list_t<Head>>,
        concat_list_t<wrap_value_elements_t<Head, lift<type_list>>, wrap_value_elements_t<Lists, lift<type_list>>...>
    > {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
