///----------------------------------
/// @file mpl/meta_callback.hpp 
/// @brief メタ関数を型テンプレートパラメータの文脈でも使用できるようにする
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP

namespace tuutil::mpl
{
    template <class T>
    concept ReturenValueMetaFunction = requires {
        T::value;
    };

    template <class T>
    concept ReturenTypeMetaFunction = requires {
        typename T::type;
    };

    template <class T>
    concept MetaFunction = ReturenValueMetaFunction<T> || ReturenTypeMetaFunction<T>;

    /**
     * @fn
     * @brief メタ関数を型と同様に扱えるようにする
     * @tparam MetaF メタ関数
    */
    template <template <class...> class MetaF> struct quote {};

    /**
     * @fn
     * @brief メタ関数引数の部分適用
     * @tparam QuotedMetaF quoteまたはbindされたメタ関数
     * @tparam PartialArgs 部分適用する型パラメータを列挙
    */
    template <class QuotedMetaF, class... PartialArgs> struct bind;
    template <template <class...> class MetaF, class... PartialArgs>
    struct bind<quote<MetaF>, PartialArgs...> {};
    // bindをさらにbind
    template <class QuotedMetaF, class... PartialArgs, class... PartialArgs2>
    struct bind<bind<QuotedMetaF, PartialArgs...>, PartialArgs2...> {};

    /**
     * @fn
     * @brief quoteまたはbindか判定
     * @tparam T 検査対象型
    */
    template <class T> struct is_meta_callback : public std::false_type {};
    template <template <class...> class MetaF>
    struct is_meta_callback<quote<MetaF>> : public std::true_type {};
    template <class QuotedMetaF, class... PartialArgs>
    struct is_meta_callback<bind<QuotedMetaF, PartialArgs...>> : public std::true_type {};

    /**
     * @fn
     * @brief quoteまたはbindか判定
     * @tparam T 検査対象型
    */
    template <class T> constexpr auto is_meta_callback_v = is_meta_callback<T>::value;

    /**
     * @brief quoteまたはbindである制約
    */
    template <class T>
    concept MetaCallable = is_meta_callback_v<T>;

    /**
     * @fn
     * @brief quoteまたはbindを渡す。 type呼び出しで実行
     * @tparam F quoteまたはbindされたメタ関数
     * @tparam Args 適用する引数
    */
    template <MetaCallable F, class... Args> struct apply;
    // 引数を渡してメタ関数実行
    template <template <class...> class MetaF, class... Args>
    struct apply<quote<MetaF>, Args...> : public MetaF<Args...>
    {
        static_assert(MetaFunction<MetaF<Args...>>, "Invalid argment. Non-metafunction specified for argument F.");
    };
    // 部分適用された引数を展開
    template <class QuotedMetaF, class... PartialArgs, class... Args>
    struct apply<bind<QuotedMetaF, PartialArgs...>, Args...> : public apply<QuotedMetaF, PartialArgs..., Args...> {};

    template <MetaCallable F, class... Args>
    requires (ReturenTypeMetaFunction<apply<F, Args...>>)
    using apply_t = apply<F, Args...>::type;

    template <MetaCallable F, class... Args>
    requires (ReturenValueMetaFunction<apply<F, Args...>>)
    constexpr auto apply_v = apply<F, Args...>::value;

    /**
     * @fn
     * @brief F<Args...>を実行した結果に対してWithFを適用する
     * @tparam WithF 適用結果に追加で適用する処理を指定
     * @tparam F メインの処理
     * @tparam Args Fに渡される引数
    */
    template <MetaCallable WithF, MetaCallable F, class... Args>
    struct apply_with : public apply<WithF, apply<F, Args...>> {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP
