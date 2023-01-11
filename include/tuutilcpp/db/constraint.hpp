///----------------------------------
/// @file constraint.hpp
/// @brief 列制約定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP

namespace tuutil::db
{
    // /**
    //  * @brief カラム定義の制約を示す定数
    // */
    // enum class column_constraints
    // {
    //     PK, FK, NOT_NULL, UNIQUE, CHECK
    // };

    struct constraint {};
    struct constraint_c {};

    // テーブル定義時の指定用
    template <mpl::Enumeration auto... ColIds>
    struct primary_key : public constraint {};
    // カラム定義時の指定用
    struct pk : public constraint_c {};

    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = requires {
        // 対象と参照先の二つ
        requires sizeof...(ColIdLists) == 2;
        // 非型パラメータパックを持つコンテナである
        requires (mpl::has_value_parameters_v<ColIdLists> && ...);
        // 対象と参照先のカラムの数は同じでなければならない
        requires (mpl::count_v<ColIdLists> == ...);
        // それぞれ、IdListが持つ値の型は全て同じでなければならない
        requires (mpl::is_same_types_v<ColIdLists> && ...);
        // それぞれ、IdListが持つ値は重複があってはならない
        requires (mpl::is_unique_v<ColIdLists> && ...);
        // 非型パラメータは列挙体でなければいけない(上記で、同じ型である確認をとっているため、先頭のみ見ればよい)
        requires (mpl::Enumeration<mpl::get_front_t<ColIdLists>> && ...);
    };

    // テーブル定義時の指定用
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};
    // カラム定義時の指定用
    template <mpl::Enumeration auto ColId> struct fk : public constraint_c {};

    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    template <class T> struct is_constraint_definition
        : public std::bool_constant<std::is_base_of_v<constraint, T>> {};

    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    template <class T>
    concept ConstraintListDefinable = requires {
        // Tはパラメータパックを持っている
        requires mpl::has_type_parameters_v<T>;

        // 同じ定義が存在してはいけない
        requires mpl::is_unique_v<T>;

        // テンプレート引数が全て制約定義であること(全てのパラメータが制約定義であるかをリストとして取得し、論理積で結果を確認)
        requires mpl::apply_list<
            mpl::quote<std::conjunction>,
            mpl::map_t<mpl::quote<is_constraint_definition>, T>
        >::value;

        // 複数のprimary_keyが存在してはいけない
    };

    /**
     * @fn
     * @brief カラム定義用から通常のテーブル定義用に制約を
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId> struct to_table_constraint;
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};
    
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * テーブルとして制約を未指定であること
    */
    using constraint_unspecified = mpl::type_list<constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
