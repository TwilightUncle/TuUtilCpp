///----------------------------------
/// @file constraint.hpp
/// @brief 列制約定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP

namespace tuutil::db
{
    /**
     * @class
     * @brief テーブルの制約として指定可能な型の基底。ただし、直接これを指定しても無視される
    */
    struct table_constraint {};

    /**
     * @class
     * @brief カラムの制約として指定可能な型の基底。ただし、直接、これを指定しても無視される
    */
    struct column_constraint {};

    /**
     * @brief カラムIDリストとして正しい形式か検査
    */
    template <class ColIdList>
    concept ColIdListSpecifiable = mpl::has_value_parameters_v<ColIdList>   // ColIdListは非型テンプレートパラメータパックを持つこと
        && mpl::is_same_types_v<ColIdList>                                  // ColIdListのパラメータパック要素の型は全て同じであること
        && mpl::is_unique_v<ColIdList>                                      // ColIdListのパラメータパックの値に重複があってはいけない
        && mpl::Enumeration<mpl::get_front_t<ColIdList>>;                   // ColIdListの型は列挙体でなければいけない

    /**
     * @class
     * @brief テーブルに主キー制約を指定する
     * @tparam ColIds 主キーとして設定するカラムIDを列挙する
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct primary_key : public table_constraint
    {
        using id_type = mpl::get_front_t<mpl::value_list<ColIds...>>;
    };

    /**
     * @fn
     * @brief 主キー制約か判定
    */
    template <class T> struct is_primary_key : public std::false_type {};
    template <mpl::Enumeration auto... ColIds>
    struct is_primary_key<primary_key<ColIds...>> : public std::true_type {};

    /**
     * @fn
     * @brief 主キー制約か判定
    */
    template <class T> constexpr auto is_primary_key_v = is_primary_key<T>::value;
    
    /**
     * @class
     * @brief カラムに主キー制約を指定する
    */
    struct pk : public column_constraint {};

    /**
     * @brief 外部キー制約の正しい引数か検査
    */
    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = (sizeof...(ColIdLists) == 2)
        && (ColIdListSpecifiable<ColIdLists> && ...)            // ColIdListsはそれぞれでColIdListSpecifiableの制約を満たさなければ伊根内
        && mpl::is_unique_v<mpl::concat_list_t<ColIdLists...>>  // 二つのIDリストに一致するIDが含まれていてはならない
        && (mpl::count_v<ColIdLists> == ...);                   // 二つのIDリストが持つパラメータパックの数は等しくなければいけない

    /**
     * @class
     * @brief テーブルに外部キー制約を指定する
     * @tparam ColIdList 外部キーとするIDの組
     * @tparam RefColIdList 外部キーとして参照するIDの組
    */
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public table_constraint
    {
        using id_type = mpl::get_front_t<ColIdList>;
    };

    /**
     * @fn
     * @brief 外部キー制約か判定
    */
    template <class T> struct is_foreign_key : public std::false_type {};
    template <class... ColIdLists>
    struct is_foreign_key<foreign_key<ColIdLists...>> : public std::true_type {};

    /**
     * @fn
     * @brief 外部キー制約か判定
    */
    template <class T> constexpr auto is_foreign_key_v = is_foreign_key<T>::value;

    /**
     * @class
     * @brief カラムに外部キー制約を指定する
     * @tparam RefColId 参照先のカラムID
    */
    template <mpl::Enumeration auto RefColId> struct fk : public column_constraint {};

    /**
     * @class
     * @brief テーブルに一意性制約を指定する
     * @tparam ColIds 一意性を持たせたい列の組
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct unique : public table_constraint
    {
        using id_type = mpl::get_front_t<mpl::value_list<ColIds...>>;
    };

    /**
     * @fn
     * @brief 一意性制約か判定
    */
    template <class T> struct is_unique : public std::false_type {};
    template <mpl::Enumeration auto... ColIds> struct is_unique<unique<ColIds...>> : public std::true_type {};

    /**
     * @class
     * @brief カラムに一意性制約を指定する
    */
    struct uni : public column_constraint {};

    /**
     * @class
     * @brief 自動採番。集計目的のため、これを直接テーブルに指定してはいけない
    */
    template <mpl::Enumeration auto ColId>
    struct auto_increment : public table_constraint
    {
        using id_type = decltype(ColId);
    };

    /**
     * @class
     * @brief カラムに自動採番を設定する
    */
    struct ai : public column_constraint {};

    /**
     * @fn
     * @brief 自動採番か判定
    */
    template <class T> struct is_auto_increment : public std::false_type {};
    template <mpl::Enumeration auto ColId>
    struct is_auto_increment<auto_increment<ColId>> : public std::true_type {};

    /**
     * @class
     * @brief NOT NULL制約
    */
    struct not_null : public column_constraint {};

    /**
     * @brief カラムへ指定可能な制約のコンセプト
    */
    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<column_constraint, T> && !std::is_same_v<column_constraint, T>;

    /**
     * @fn
     * @brief テーブルの制約か判定
    */
    template <class T> using is_constraint_definition = std::is_base_of<table_constraint, T>;

    /**
     * @brief テーブルの制約であること
    */
    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    /**
     * @fn
     * @brief テーブルの識別子型を取得する
    */
    template <class T> struct get_table_id;
    template <ConstraintDefinable T>
    struct get_table_id<T> : public std::type_identity<typename T::id_type> {};

    /**
     * @brief テーブルへの制約リストであること
    */
    template <class T>
    concept ConstraintListDefinable = mpl::has_type_parameters_v<T>                                             // Tは型のパラメータパックを持つこと
        && mpl::is_unique_v<T>                                                                                  // Tの持つパラメータパックに重複はあってはならない
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_constraint_definition>, T>> // Tの持つパラメータパック要素は全てconstraint_definitionでなければならない
        && mpl::is_same_types_v<
            mpl::map_t<
                mpl::quote<get_table_id>,
                mpl::remove_if_t<mpl::bind<mpl::quote<std::is_same>, table_constraint>, T>
            >>                                                                                                  // Tが持つパラメータパック要素のメンバid_typeは全て同じ型であること(基底型を除く)
        && mpl::count_if_v<mpl::quote<is_primary_key>, T> < 2                                                   // 複数の主キーが一つのテーブルに存在してはいけない
        && mpl::count_if_v<mpl::quote<is_auto_increment>, T> < 2;                                               // 複数の自動採番列が一つのテーブルに存在してはいけない

    /**
     * @fn
     * @brief カラム定義用から通常のテーブル定義用に制約を変換する
     * @tparam Constraint カラムに指定している制約
     * @tparam ColId Constraintを指定している対象のカラムID
     * @details カラムのみで特にテーブルに指定するものではない場合、デフォルトでtable_constraintを返す
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    struct to_table_constraint : public std::type_identity<table_constraint> {};

    // 各制約に対する特殊化
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<uni, ColId> : public std::type_identity<unique<ColId>> {};
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<ai, ColId> : public std::type_identity<auto_increment<ColId>> {};

    /**
     * @fn
     * @brief カラム定義用から通常のテーブル定義用に制約を変換する
     * @tparam Constraint カラムに指定している制約
     * @tparam ColId Constraintを指定している対象のカラムID
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * テーブルとして制約を未指定であること
    */
    using constraint_unspecified = mpl::type_list<table_constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
