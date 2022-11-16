///----------------------------------
/// @file table.hpp
/// @brief DBテーブル定義
///----------------------------------
#pragma once
#include <column.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief テーブル定義用メタ関数
     * @param ETableType テーブルと列の識別子として定義したスコープ付き列挙型
     * @param Name cstrで包んで渡した文字列リテラル
     * @param ColumnDefinitionList define_columnによる列定義を指定する
    */
    template <
        enumeration ETableType,
        StringLiteralSpecificable auto Name,
        ColumnListDefinitionable ColumnDefinitionList
    >
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
