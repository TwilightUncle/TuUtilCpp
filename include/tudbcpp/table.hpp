///----------------------------------
/// @file table.hpp
/// @brief DBテーブル定義
///----------------------------------
#pragma once
#include <tudbcpp/column.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief テンプレート引数検証用のメタ関数
    */
    template <
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct validate_define_table_constraint_arg : public std::bool_constant<
        ConstraintListDefinable<
            concat_type_list_t<
                ConstraintDefinitionList,
                extract_constraints_t<ColumnDefinitionList>
            >
        >
    > {};

    /**
     * @fn
     * @brief テーブル定義用メタ関数
     * @tparam ETableType テーブルと列の識別子として定義したスコープ付き列挙型
     * @tparam Name tustr::cstrで包んで渡した文字列リテラル
     * @tparam ColumnDefinitionList define_columnによる列定義を指定する
     * @tparam ConstraintDefinitionList define_columnによる列定義を指定する
    */
    template <
        enumeration ETableType,
        tustr::cstr Name,
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList = constraint_unspecified
    >
    requires validate_define_table_constraint_arg<ColumnDefinitionList, ConstraintDefinitionList>::value
    struct define_table
    {
    private:
        template <auto V, ColumnDefinable T>
        requires std::same_as<ETableType, decltype(V)>
        struct is_match_column_id : public std::bool_constant<V == T::id> {};

    public:
        static constexpr auto name = Name;

        /**
         * @fn
         * @brief Vに合致する列定義を取得する
        */
        template <ETableType V>
        using get_column_definition_t = find_if_by_value_t<V, is_match_column_id, pass_types<ColumnDefinitionList>>;

        /**
         * @fn
         * @brief Vに合致する列情報を取得する
        */
        template <ETableType V>
        static constexpr auto get_column_info()
        {
            return get_column_definition_t<V>{};
        }
    };
}
