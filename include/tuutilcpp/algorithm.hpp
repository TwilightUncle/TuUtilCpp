///----------------------------------
/// @file algorithm.hpp 
/// @brief 汎用メタ関数定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP
#define TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP

namespace tudb
{
    // type_list関連の前方宣言
    struct ignore_type;
    template <class... Types> struct type_list;
    template <class T> struct pass_types;
    template <class T1, class T2> struct push_type;

    /**
     * @fn
     * @brief メタ関数を実体化済みの型に丸める
    */
    template <template <class...> class F> struct quote {};

    /**
     * @fn
     * @brief テンプレートパラメータの部分適用
    */
    template <class RoundedF, class... PartialArgs> struct bind;
    template <template <class...> class F, class... PartialArgs>
    struct bind<quote<F>, PartialArgs...> {};
    // bindをさらにbind
    template <class RoundedF, class... PartialArgs, class... PartialArgs2>
    struct bind<bind<RoundedF, PartialArgs...>, PartialArgs2...> {};

    /**
     * @fn
     * @brief メタ関数を丸めこんだものか判定
    */
    template <class T> struct is_rounded_meta_function : public std::false_type {};
    template <template <class...> class F>
    struct is_rounded_meta_function<quote<F>> : public std::true_type {};
    template <class RoundedF, class... PartialArgs>
    struct is_rounded_meta_function<bind<RoundedF, PartialArgs...>> : public std::true_type {};

    /**
     * @fn
     * @brief メタ関数を丸めこんだものか判定
    */
    template <class T> constexpr auto is_rounded_meta_function_v = is_rounded_meta_function<T>::value;

    template <class T>
    concept MetaCallable = is_rounded_meta_function_v<T>;

    /**
     * @fn
     * @brief quoteまたはbindを渡す。 type呼び出しで実行
    */
    template <MetaCallable F, class... Args> struct apply;
    // 引数を渡して実行
    template <template <class...> class F, class... Args>
    struct apply<quote<F>, Args...> : public F<Args...> {};
    // 再帰的に部分適用された引数を展開
    template <class RoundedF, class... PartialArgs, class... Args>
    struct apply<bind<RoundedF, PartialArgs...>, Args...> : public apply<RoundedF, PartialArgs..., Args...> {};

    template <MetaCallable F, class... Args> using apply_t = apply<F, Args...>::type; 

    /**
     * @fn
     * @brief 受け取ったパラメータパックの先頭を取得するメタ関数
    */
    template <class Head, class... Parameters>
    struct get_first_type : public std::type_identity<Head> {};

    /**
     * @fn
     * @brief 左から右へ畳み込み
    */
    template <MetaCallable F, class... Parameters> struct foldl;
    template <MetaCallable F, class Init, class Head, class... Parameters>
    struct foldl<F, Init, Head, Parameters...> : public foldl<F, apply<F, Init, Head>, Parameters...> {};
    template <MetaCallable F, class Init, class Head>
    struct foldl<F, Init, Head> : public apply<F, Init, Head> {};

    template <MetaCallable F, class... Parameters> using foldl_t = foldl<F, Parameters...>::type;

    /**
     * @fn
     * @brief 右から左へ畳み込み
    */
    template <MetaCallable F, class... Parameters> struct foldr;
    template <MetaCallable F, class Head, class... Parameters>
    struct foldr<F, Head, Parameters...> : public apply<F, Head, foldr<F, Parameters...>> {};
    template <MetaCallable F, class Head, class Next>
    struct foldr<F, Head, Next> : public apply<F, Head, Next> {};

    template <MetaCallable F, class... Parameters> using foldr_t = foldr<F, Parameters...>::type;


    /**
     * @fn
     * @brief パラメータパックに対し、左から右へ畳み込みを行い、一つの型を返す
    */
    template <template <class...> class Op, class... Parameters> struct foldl_by_type;
    // 2引数関数の場合
    template <template <class, class> class Op, class Head, class Next, class... Parameters>
    requires TwoTypeArgsTypeGetable<Op, Head, Next>
    struct foldl_by_type<Op, Head, Next, Parameters...>
        : public foldl_by_type<Op, typename Op<Head, Next>::type, Parameters...> {};
    // 結果
    template <template <class, class> class Op, class Param>
    struct foldl_by_type<Op, Param> : public std::type_identity<Param> {};

    /**
     * @fn
     * @brief パラメータパックに対し、右から左へ畳み込みを行い、一つの型を返す
    */
    template <template <class...> class Op, class... Parameters> struct foldr_by_type;
    template <template <class, class> class Op, class Head, class... Parameters>
    requires TwoTypeArgsTypeGetable<Op, Head, typename foldr_by_type<Op, Parameters...>::type>
    struct foldr_by_type<Op, Head, Parameters...> : public Op<typename foldr_by_type<Op, Parameters...>::type, Head> {};
    // スタート
    template <template <class, class> class Op, class Param>
    struct foldr_by_type<Op, Param> : public std::type_identity<Param> {}; 

    /**
     * @fn
     * @brief 全てのParametersにFnを適用した結果の型リストを返す
    */
    template <template <class> class Fn, class... Parameters>
    requires (TypeGetable<Fn<Parameters>> && ...)
    struct map_types : std::type_identity<type_list<typename Fn<Parameters>::type...>> {};

    /**
     * @fn
     * @brief 第一型引数と第三引数で渡された型のパラメータパックを順番にCondで比較し、最初に結果が真であったパラメータを取得するメタ関数
     * @param T 検索条件の引数
     * @param Cond 判定に使用するメタ関数
     * @param Head ここにパラメータパックを指定
     * @return 最初に発見した型を返却。見つからなかった場合voidを返却
     * @details (e.g 下記の場合、resはstd::true_typeとなる
     * @details using res = find_if_by_type<std::bool_constant<true>, std::is_base_of, int, float, std::true_type, double>::type;
    */
    template <class T, template <class, class> class Cond, class... Parameters> struct find_if_by_type;
    template <class T, template <class, class> class Cond, class Head, class... Parameters>
    requires TwoTypeArgsTestable<Cond, T, Head>
    struct find_if_by_type<T, Cond, Head, Parameters...>
        : public std::conditional_t<Cond<T, Head>::value,
            std::type_identity<Head>,
            find_if_by_type<T, Cond, Parameters...>>
    {};
    // 再帰の終点(なかったのでvoid)
    template <class T, template <class, class> class Cond>
    struct find_if_by_type<T, Cond> : public std::type_identity<void> {};

    /**
     * @fn
     * @brief 第一型引数と第三引数で渡された型のパラメータパックを順番にCondで比較し、最初に結果が真であったパラメータを取得するメタ関数
     * @param V 検索条件の引数
     * @param Cond 判定に使用するメタ関数
     * @param Head ここにパラメータパックを指定
     * @return なかったらvoid
    */
    template <auto V, template <auto, class> class Cond, class... Parameters> struct find_if_by_value;
    template <auto V, template <auto, class> class Cond, class Head, class... Parameters>
    requires FirstValueSecondTypeArgsTestable<Cond, V, Head>
    struct find_if_by_value<V, Cond, Head, Parameters...>
        : public std::conditional_t<Cond<V, Head>::value,
            std::type_identity<Head>,
            find_if_by_value<V, Cond, Parameters...>>
    {};
    // 再帰の終点(発見できずvoidを返却)
    template <auto V, template <auto, class> class Cond>
    struct find_if_by_value<V, Cond> : public std::type_identity<void> {};

    /**
     * @fn
     * @brief 条件に合致する型を取り除く
     * @tparam T 比較対象の型
     * @tparam Cond 条件式(第一引数にはTが入る)
     * @tparam Parameters 処理対象のパラメータパック
    */
    template <class T, template <class, class> class Cond, class... Parameters>
    struct remove_if_by_type
        : public foldl_by_type<
            push_type,
            type_list<ignore_type>, // init
            std::conditional_t<Cond<T, Parameters>::value, ignore_type, Parameters>... // 条件に合致するものをignore_typeへ置換
        >
    {};

    /**
     * @fn
     * @brief 2値の非型テンプレート引数を比較するメタ関数。型一致かつ、値一致の場合真
    */
    template <auto V1, auto V2> struct equal_values : public std::false_type {};
    template <auto V1, auto V2>
    requires std::same_as<decltype(V1), decltype(V2)>
    struct equal_values<V1, V2> : public std::bool_constant<V1 == V2> {};

    /**
     * @fn
     * @brief 第一引数が第二以降の引数のいずれかに一致している場合真
    */
    template <class Test, class... Parameters>
    struct include_type : public std::bool_constant<(std::is_same_v<Test, Parameters> || ...)> {};

    /**
     * @fn
     * @brief 第一引数が第二以降の引数のいずれかに一致している場合真
    */
    template <auto Test, auto... Parameters>
    struct include_value : public std::bool_constant<(equal_values<Test, Parameters>::value || ...)> {};

    /**
     * @fn
     * @brief 受け取ったテンプレートパラメータパックの要素に重複が存在しないか判定するメタ関数
    */
    template <class Head, class... Parameters>
    struct is_unique
        : public std::bool_constant<!include_type<Head, Parameters...>::value && is_unique<Parameters...>::value>
    {};
    template <class Param> struct is_unique<Param> : public std::true_type {};

    /**
     * @fn
     * @brief 受け取った非型テンプレートパラメータパックの要素に重複が存在しないか判定するメタ関数
     * @details 型が異なっていれば不一致とする。型が一致していた場合は値を比較し、異なっていれば不一致とする
    */
    template <auto Head, auto... Parameters>
    struct is_unique_values
        : public std::bool_constant<!include_value<Head, Parameters...>::value && is_unique_values<Parameters...>::value>
    {};
    template <auto Param> struct is_unique_values<Param> : public std::true_type {};

    /**
     * @fn
     * @brief パラメータパックの型が全て同じとき真
    */
    template <class Head, class... Parameters>
    struct is_same_types
        : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};

    /**
     * @fn
     * @brief パラメータパックの値の型が全て同じ時真
    */
    template <auto... Parameters>
    struct is_same_value_types : public is_same_types<decltype(Parameters)...> {};

    /**
     * @fn
     * @brief テンプレート引数の数を数える
    */
    template <class... Types>
    struct count_parameters : public std::integral_constant<std::size_t, sizeof...(Types)> {};

    /**
     * @fn
     * @brief テンプレート引数の数を数える
    */
    template <auto... Values>
    struct count_value_parameters : public std::integral_constant<std::size_t, sizeof...(Values)> {};

    /**
     * @fn
     * @brief 二つの型の領域のサイズが等しいか検証
    */
    template <class T1, class T2>
    struct equal_type_size : public std::bool_constant<sizeof(T1) == sizeof(T2)> {};

    /**
     * @fn
     * @brief テンプレート引数で渡した整数値の桁数を取得する。第二引数は任意の進数を指定する
    */
    template <auto V, std::size_t Hex = 10>
    requires (Hex >= 2 && (std::integral<decltype(V)> || is_big_int<decltype(V)>::value))
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }

    //-----------------------------------------------------
    // 以下、_t定義
    //-----------------------------------------------------

    template <class... Parameters>
    using get_first_type_t = get_first_type<Parameters...>::type;

    template <template <class...> class Op, class... Parameters>
    using foldl_by_type_t = foldl_by_type<Op, Parameters...>::type;

    template <template <class...> class Op, class... Parameters>
    using foldr_by_type_t = foldr_by_type<Op, Parameters...>::type;

    template <template <class> class Fn, class... Parameters>
    using map_types_t = map_types<Fn, Parameters...>::type;

    template <class T, template <class, class> class Cond, class... Parameters>
    using find_if_by_type_t = find_if_by_type<T, Cond, Parameters...>::type;

    template <auto V, template <auto, class> class Cond, class... Parameters>
    using find_if_by_value_t = find_if_by_value<V, Cond, Parameters...>::type;

    template <class T, template <class, class> class Cond, class... Parameters>
    using remove_if_by_type_t = remove_if_by_type<T, Cond, Parameters...>::type;

    //-----------------------------------------------------
    // 以下、_v定義
    //-----------------------------------------------------

    template <auto V1, auto V2>
    constexpr auto equal_values_v = equal_values<V1, V2>::value;

    template <class Test, class...Parameters>
    constexpr auto include_type_v = include_type<Test, Parameters...>::value;

    template <auto Test, auto... Parameters>
    constexpr auto include_value_v = include_value<Test, Parameters...>::value;

    template <class... Parameters>
    constexpr auto is_unique_v = is_unique<Parameters...>::value;

    template <auto... Parameters>
    constexpr auto is_unique_values_v = is_unique_values<Parameters...>::value;

    template <class... Parameters>
    constexpr auto is_same_types_v = is_same_types<Parameters...>::value;

    template <auto... Parameters>
    constexpr auto is_same_value_types_v = is_same_value_types<Parameters...>::value;

    template <class... Types>
    constexpr auto count_parameters_v = count_parameters<Types...>::value;

    template <auto... Values>
    constexpr auto count_value_parameters_v = count_value_parameters<Values...>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_ALGORITHM_HPP
