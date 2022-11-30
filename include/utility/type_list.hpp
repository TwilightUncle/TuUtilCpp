#pragma once

namespace tudb
{
    /**
     * @class
     * @brief 型のコンテナ。パラメータパックにより操作
    */
    template <class... Types> struct type_list
    {
        static constexpr auto size = sizeof...(Types);
    };

    /**
     * @fn
     * @brief type_listか判定
    */
    template <class T> struct is_type_list : public std::false_type {};
    template <class... Types> struct is_type_list<type_list<Types...>> : public std::true_type {};
    template <class T> constexpr auto is_type_list_v = is_type_list<T>::value;

    /**
     * @fn
     * @brief 本ライブラリ内で、テンプレートパラメータを抜き出すときの識別子
    */
    template <class T> struct pass_types;
    template <template <class...> class T, class... Parameters> struct pass_types<T<Parameters...>> {};

    /**
     * @fn
     * @brief Srcのパラメータパックの内容がDestのテンプレート引数として展開される
    */
    template <class Src, template <class...> class Dest> struct copy_types;
    template <template <class...> class Src, template <class...> class Dest, class... Parameters>
    struct copy_types<Src<Parameters...>, Dest> : public std::type_identity<Dest<Parameters...>> {};

    template <class Src, template <class...> class Dest> using copy_types_t = copy_types<Src, Dest>::type;

    /**
     * @fn
     * @brief 型のパラメータパックを持つか判定
    */
    template <class T> struct has_type_parameters : public std::false_type {};
    template <template <class...> class T, class... Parameters>
    struct has_type_parameters<T<Parameters...>> : public std::true_type {};

    template <class T> constexpr auto has_type_parameters_v = has_type_parameters<T>::value;

    /**
     * @fn
     * @brief パラメータパックの前後に新たにTを加える
    */
    template <class T1, class T2> struct push_type;
    // 先頭に追加バージョン
    template <class T, template <class...> class Container, class... Types>
    struct push_type<T, Container<Types...>> : public std::type_identity<Container<T, Types...>> {};
    // 末尾に追加バージョン
    template <class T, template <class...> class Container, class... Types>
    struct push_type<Container<Types...>, T> : public std::type_identity<Container<Types..., T>> {};

    template <class T1, class T2> using push_type_t = push_type<T1, T2>::type;

    /**
     * @fn
     * @brief 1つ以上のパラメータパックの内容を結合する
    */
    template <class... Lists>
    requires (sizeof...(Lists) > 0)
    struct concat_type_list;
    // 二つのtype_listを結合
    template <template <class...> class Container, class... Types1, class... Types2>
    struct concat_type_list<Container<Types1...>, Container<Types2...>> : std::type_identity<Container<Types1..., Types2...>> {};
    // 任意数のtype_listを結合する
    template <class... Lists>
    requires (has_type_parameters_v<Lists> && ...)
    struct concat_type_list<Lists...> : public foldl_by_type<concat_type_list, Lists...> {};

    template <class... Lists> using concat_type_list_t = concat_type_list<Lists...>::type;

    //-----------------------------------------------------
    // 以下、algorithmのパラメータパック抜き取り版
    //-----------------------------------------------------

    template <template <class...> class Container, class... Parameters>
    struct get_first_type<pass_types<Container<Parameters...>>> : public get_first_type<Parameters...> {};

    template <template <class...> class Op, template <class...> class Container, class... Parameters>
    struct foldl_by_type<Op, pass_types<Container<Parameters...>>> : public foldl_by_type<Op, Parameters...> {};

    template <template <class...> class Op, template <class...> class Container, class... Parameters>
    struct foldr_by_type<Op, pass_types<Container<Parameters...>>> : public foldr_by_type<Op, Parameters...> {};

    template <template <class> class Fn, template <class...> class Container, class... Parameters>
    struct map_types<Fn, pass_types<Container<Parameters...>>> : public map_types<Fn, Parameters...> {};

    template <class T, template <class, class> class Cond, template <class...> class Container, class... Parameters>
    struct find_if_by_type<T, Cond, pass_types<Container<Parameters...>>> : public find_if_by_type<T, Cond, Parameters...> {};

    template <auto V, template <auto, class> class Cond, template <class...> class Container, class... Parameters>
    struct find_if_by_value<V, Cond, pass_types<Container<Parameters...>>> : public find_if_by_value<V, Cond, Parameters...> {};

    template <template <class...> class Container, class... Parameters>
    struct count_parameters<pass_types<Container<Parameters...>>> : public count_parameters<Parameters...> {};

    template <template <class...> class Container, class... Parameters>
    struct is_unique<pass_types<Container<Parameters...>>> : public is_unique<Parameters...> {};

    template <template <class...> class Container, class... Parameters>
    struct is_same_types<pass_types<Container<Parameters...>>> : public is_same_types<Parameters...> {};
}
