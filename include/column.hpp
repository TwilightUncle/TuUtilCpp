///----------------------------------
/// @file column.hpp
/// @brief DB列定義
///----------------------------------
#pragma once
#include <constraint.hpp>
#include <type.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief 列定義用メタ関数(同時に定義内容参照用クラス)
    */
    template <
        enumeration auto ColID,
        StringLiteralSpecificable auto Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    struct define_column
    {
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;
        using constraint_list = type_list<to_table_constraint_t<Constraints, ColID>...>;
    };

    /**
     * @fn
     * @brief column_definition型かどうかを判定するメタ関数
    */
    template <class T> struct is_column_definition : public std::false_type {};
    template <enumeration auto ColID, StringLiteralSpecificable auto Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_columnであることを強制する制約
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    template <ColumnDefinable T>
    struct get_column_id : public std::integral_constant<decltype(T::id), T::id> {};

    template <ColumnDefinable T>
    struct get_column_name { static constexpr auto value = T::name; };

    /**
     * カラム定義リストであること
    */
    template <class T>
    concept ColumnListDefinitionable = requires {
        // Tはパラメータパックを持っている
        requires has_type_parameters_v<T>;

        // 同じ定義のカラムが存在してはいけない
        requires copy_types_t<T, is_unique>::value;

        // テンプレート引数が全てカラム定義であること(全てのパラメータがカラム定義であるかをリストとして取得し、論理積で結果を確認)
        requires copy_types_t<
            map_types_t<is_column_definition, pass_types<T>>,
            std::conjunction
        >::value;
    };
}
