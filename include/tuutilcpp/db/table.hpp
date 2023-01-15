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
        && std::is_same_v<typename mpl::get_front_t<ColumnDefinitionList>::id_type, ETableType>
        && validate_sql_identity<Name>()
    )
    struct define_table
    {
        using id_type = ETableType;
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
    struct get_column_def<ColID, define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public get_column_def<ColID, ColumnDefinitionList> {};

    /**
     * @fn
     * @brief テーブル定義クラスか判定する
    */
    template <class T> struct is_table_definition : public std::false_type {};
    template <
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct is_table_definition<define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public std::true_type {};

    /**
     * @brief define_tableであることの制約
    */
    template <class T>
    concept TableDefinable = is_table_definition<T>::value;

    /**
     * @fn
     * @brief テーブル定義からテーブル名の特性クラスを取得
    */
    template <TableDefinable T>
    struct get_table_name : public std::type_identity<mpl::value_constant<T::name>> {};

    /**
     * @fn
     * @brief テーブル定義からテーブルを識別する列挙型を取得する
    */
    template <TableDefinable T>
    struct get_table_id : public std::type_identity<typename T::id_type> {};
    
    /**
     * @fn
     * @brief テーブル定義からテーブルを識別する列挙型を取得する
    */
    template <class T> using get_talbe_id_t = get_table_id<T>::type;

    /**
     * @fn
     * @brief テーブル定義からテーブル名の特性クラスを取得
    */
    template <class T> constexpr auto get_table_name_v = get_table_name<T>::value;

    template <class T>
    concept TableListDefinable = mpl::has_type_parameters_v<T>                                              // Tは型のパラメータパックを持つこと
        && mpl::is_unique_v<T>                                                                              // Tが持つパラメータパックは重複がないこと
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_table_definition>, T>>  // Tが持つパラメータパックは全てdefine_tableであること
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_table_id>, T>>                                        // Tが持つパラメータパック要素のメンバidは全て異なる型であること
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_table_name>, T>>;                                     // Tが持つパラメータパック要素のメンバnameは全て異なる値であること
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
