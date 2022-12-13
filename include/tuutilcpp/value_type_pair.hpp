///----------------------------------
/// @file tuutilcpp/value_type_pair.hpp
/// @brief テンプレート引数操作系のヘルパ群
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP
#define TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP

namespace tudb
{
    /**
     * @class
     * @brief 値と型の組
    */
    template <auto V, class T>
    struct value_type_pair : public std::type_identity<T> {
        static constexpr auto key = V;
    };

    /**
     * @fn
     * @brief 値と型の組であるか判定
    */
    template <class T> struct is_value_type_pair : public std::false_type {};
    template <auto V, class T>
    struct is_value_type_pair<value_type_pair<V, T>> : public std::true_type {};

    /**
     * 指定した値と、値と型の組のキーが等しいか判定
    */
    template <auto V, class T> struct equal_key : public std::false_type {};
    template <auto V, auto Key, class T>
    struct equal_key<V, value_type_pair<Key, T>> : public equal_values<V, Key> {};

    template <class T>
    concept AssociativeTypeList = requires {
        requires has_type_parameters_v<T>;

        // テンプレート引数が値と型の組であること(全てのパラメータがカラム定義であるかをリストとして取得し、論理積で結果を確認)
        requires copy_types_t<
            map_types_t<is_value_type_pair, pass_types<T>>,
            std::conjunction
        >::value;
    };

    /**
     * @fn
     * @brief 型の連想配列からキーが一致する要素を取得する
    */
    template <auto SearchKey, AssociativeTypeList List>
    struct get : public find_if_by_value<SearchKey, equal_key, pass_types<List>>::type {};
    template <auto SearchKey, AssociativeTypeList List> using get_t = get<SearchKey, List>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_VALUE_TYPE_PAIR_HPP
