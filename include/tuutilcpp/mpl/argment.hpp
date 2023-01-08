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
    template <MetaCallable F, class Arg2, class Arg1>
    struct flip : public apply<F, Arg1, Arg2> {};

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
     * @brief リストとして渡された型リストを引数として関数に適用する
     * @tparam F メタ関数
     * @tparam ArgList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class ArgList> struct apply_list {};
    template <MetaCallable F, template <class...> class List, class... Args>
    struct apply_list<F, List<Args...>> : public apply<F, Args...> {};

    /**
     * @fn
     * @brief リストとして渡された型リストを引数として関数に適用する
     * @tparam F メタ関数
     * @tparam ArgList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class ArgList> using apply_list_t = apply_list<F, ArgList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
