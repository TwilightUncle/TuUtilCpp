///----------------------------------
/// @file table.hpp
/// @brief DBテーブル定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP

namespace tuutil::db
{
    /**
     * @fn
     * @brief テーブル定義用メタ関数
     * @tparam ETableType テーブルと列の識別子として定義したスコープ付き列挙型
     * @tparam Name str::cstrで包んで渡した文字列リテラル
     * @tparam ColumnDefinitionList define_columnによる列定義を指定する
     * @tparam ConstraintDefinitionList define_columnによる列定義を指定する
    */
    template <
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList = constraint_unspecified
    >
    requires (
        ConstraintListDefinable<
            mpl::concat_list_t<
                ConstraintDefinitionList,
                extract_constraints_t<ColumnDefinitionList>
            >
        >
        && validate_sql_identity<Name>()
    )
    struct define_table
    {
        static constexpr auto name = Name;
    };

    // テーブル定義からカラム定義を取得する場合の特殊化
    template <
        mpl::Enumeration auto ColID,
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct get_column_definition<ColID, define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public get_column_definition<ColID, ColumnDefinitionList> {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
