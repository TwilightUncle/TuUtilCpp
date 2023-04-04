///----------------------------------
/// @file mpl/map.hpp 
/// @brief メタ関数による型パラメータパックの一括適用
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class TypeList> struct map;
    template <MetaCallable F, template <class...> class List, class... Parameters>
    struct map<F, List<Parameters...>> : public std::type_identity<List<apply_t<F, Parameters>...>> {};
    template <MetaCallable F>
    struct map<F, ignore_type> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable F, class TypeList> using map_t = typename map<F, TypeList>::type;

    /**
     * @fn
     * @brief 条件に合致するパラメータパック全てにメタ関数を適用する
     * @tparam Pred 判定用関数
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, MetaCallable F, class TypeList> using map_if = map<bind<quote<apply_if>, Pred, F>, TypeList>;

    /**
     * @fn
     * @brief 条件に合致するパラメータパック全てにメタ関数を適用する
     * @tparam Pred 判定用関数
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
    */
    template <MetaCallable Pred, MetaCallable F, class TypeList> using map_if_t = typename map_if<Pred, F, TypeList>::type;

    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用を行い、全ての要素を値に変更する
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam NewList 変換後の非型要素を格納するリスト
    */
    template <MetaCallable F, class TypeList, LiftedvList NewList = liftv<value_list>> struct t_v_map;
    template <MetaCallable F, template <class...> class List, template <auto...> class NewList, class... Parameters>
    struct t_v_map<F, List<Parameters...>, liftv<NewList>> : public std::type_identity<NewList<apply_v<F, Parameters>...>> {};
    template <MetaCallable F, LiftedvList NewList>
    struct t_v_map<F, ignore_type, NewList> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief パラメータパックに対するメタ関数の一括適用を行い、全ての要素を値に変更する
     * @tparam F 適用するメタ関数
     * @tparam TypeList 型のパラメータパックを持つ型
     * @tparam NewList 変換後の非型要素を格納するリスト
    */
    template <MetaCallable F, class TypeList, LiftedvList NewList = liftv<value_list>> using t_v_map_t = typename t_v_map<F, TypeList, NewList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
