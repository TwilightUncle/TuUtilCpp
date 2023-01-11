///----------------------------------
/// @file mpl/reverse.hpp 
/// @brief 型パラメータパックの並びを反転
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief std::conditionalのラッパー。判定関数と検査対象を渡し、遅延評価できるようにしたもの
     * @tparam Pred 判定関数
     * @tparam Arg Predに食わせる判定対象型
     * @tparam T apply_v<Pred, Arg>が真の場合返却される型
     * @tparam F apply_v<Pred, Arg>が偽の場合返却される型
    */
    template <MetaCallable Pred, class Arg, class T, class F>
    using mpl_if = std::conditional<apply_v<Pred, Arg>, T, F>;

    /**
     * @fn
     * @brief std::conditionalのラッパー。判定関数と検査対象を渡し、遅延評価できるようにしたもの
     * @tparam Pred 判定関数
     * @tparam Arg Predに食わせる判定対象型
     * @tparam T apply_v<Pred, Arg>が真の場合返却される型
     * @tparam F apply_v<Pred, Arg>が偽の場合返却される型
    */
    template <MetaCallable Pred, class Arg, class T, class F> using mpl_if_t = mpl_if<Pred, Arg, T, F>::type;

    /**
     * @fn
     * @brief 引数が指定条件に合致する場合のみ、メタ関数を適用する
     * @tparam Pred 引数を検査する関数
     * @tparam F メタ関数
     * @tparam Arg 引数
    */
    template <MetaCallable Pred, MetaCallable F, class Arg>
    using apply_if = mpl_if<Pred, Arg, apply_t<F, Arg>, Arg>;

    /**
     * @fn
     * @brief 引数が指定条件に合致する場合のみ、メタ関数を適用する
     * @tparam Pred 引数を検査する関数
     * @tparam F メタ関数
     * @tparam Arg 引数
    */
    template <MetaCallable Pred, MetaCallable F, class Arg> using apply_if_t = apply_if<Pred, F, Arg>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_IF_HPP
