///----------------------------------
/// @file constraint.hpp
/// @brief 列制約定義
///----------------------------------
#ifndef TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP
#define TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP

#include <tuutilcpp/utility.hpp>

namespace tudb
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
    template <enumeration auto... ColIds>
    struct primary_key : public constraint {};
    // カラム定義時の指定用
    struct pk : public constraint_c {};

    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = requires {
        // 対象と参照先の二つ
        requires sizeof...(ColIdLists) == 2;
        // 非型パラメータパックを持つコンテナである
        requires (has_value_parameters_v<ColIdLists> && ...);
        // 対象と参照先のカラムの数は同じでなければならない
        requires (count_parameters_v<pass_values<ColIdLists>> == ...);
        // それぞれ、IdListが持つ値の型は全て同じでなければならない
        requires (is_same_types_v<pass_values<ColIdLists>> && ...);
        // それぞれ、IdListが持つ値は重複があってはならない
        requires (is_unique_v<pass_values<ColIdLists>> && ...);
        // 非型パラメータは列挙体でなければいけない(上記で、同じ型である確認をとっているため、先頭のみ見ればよい)
        requires (enumeration<get_first_type_t<pass_values<ColIdLists>>> && ...);
    };

    // テーブル定義時の指定用
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};
    // カラム定義時の指定用
    template <enumeration auto ColId> struct fk : public constraint_c {};

    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    template <class T> struct is_constraint_definition
        : public std::bool_constant<std::is_base_of_v<constraint, T>> {};

    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    template <class T>
    concept ConstraintListDefinable = requires {
        // Tはパラメータパックを持っている
        requires has_type_parameters_v<T>;

        // 同じ定義が存在してはいけない
        requires copy_types_t<T, is_unique>::value;

        // テンプレート引数が全て制約定義であること(全てのパラメータが制約定義であるかをリストとして取得し、論理積で結果を確認)
        requires copy_types_t<
            map_types_t<is_constraint_definition, pass_types<T>>,
            std::conjunction
        >::value;

        // 複数のprimary_keyが存在してはいけない
    };

    /**
     * @fn
     * @brief カラム定義用から通常のテーブル定義用に制約を
    */
    template <ColumnConstraintDefinable Constraint, enumeration auto ColId> struct to_table_constraint;
    template <enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <enumeration auto ColId, enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<value_list<ColId>, value_list<ColId2>>> {};
    
    template <ColumnConstraintDefinable Constraint, enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * テーブルとして制約を未指定であること
    */
    using constraint_unspecified = type_list<constraint>;
}

#endif // TUDBCPP_INCLUDE_GUARD_CONSTRAINT_HPP
