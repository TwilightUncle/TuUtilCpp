///----------------------------------
/// @file mpl/fold.hpp 
/// @brief メタ関数による型パラメータパックの畳み込み
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief 左から右への畳み込み内部実装
    */
    template <MetaCallable F, class... Parameters> struct foldl_impl;
    template <MetaCallable F, class Init, class Head, class... Parameters>
    struct foldl_impl<F, Init, Head, Parameters...> : public foldl_impl<F, apply_t<F, Init, Head>, Parameters...> {};
    template <MetaCallable F, class Init>
    struct foldl_impl<F, Init> : public std::type_identity<Init> {};

    /**
     * @fn
     * @brief 左から右への畳み込み
     * @tparam F 畳み込みで実行するメタ関数
     * @tparam Init 初期化子
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class Init, class TypeList> struct foldl;
    template <MetaCallable F, class Init, template <class...> class List, class... Parameters>
    struct foldl<F, Init, List<Parameters...>> : public foldl_impl<F, Init, Parameters...> {};

    /**
     * @fn
     * @brief 左から右への畳み込みを行い、結果の型を取得する
     * @tparam F 畳み込みで実行するメタ関数
     * @tparam Init 初期化子
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class Init, class TypeList> using foldl_t = foldl<F, Init, TypeList>::type;

    /**
     * @fn
     * @brief 右から左への畳み込み内部実装
    */
    template <MetaCallable F, class... Parameters> struct foldr_impl;
    template <MetaCallable F, class Head, class... Parameters>
    struct foldr_impl<F, Head, Parameters...> : public apply<F, Head, typename foldr_impl<F, Parameters...>::type> {};
    template <MetaCallable F, class Head, class Next>
    struct foldr_impl<F, Head, Next> : public apply<F, Head, Next> {};

    /**
     * @fn
     * @brief 右から左への畳み込み
     * @tparam F 畳み込みで実行するメタ関数
     * @tparam Init 初期化子
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class Init, class TypeList> struct foldr;
    template <MetaCallable F, class Init, template <class...> class List, class... Parameters>
    struct foldr<F, Init, List<Parameters...>> : public foldr_impl<F, Parameters..., Init> {};

    /**
     * @fn
     * @brief 右から左への畳み込み
     * @tparam F 畳み込みで実行するメタ関数
     * @tparam Init 初期化子
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class Init, class TypeList> using foldr_t = foldr<F, Init, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_FOLD_HPP
