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
    struct constraint {};

    /**
     * @class
     * @brief カラムの制約として指定可能な型の基底。ただし、直接、これを指定しても無視される
    */
    struct constraint_c {};

    /**
     * @brief カラムIDリストとして正しい形式か検査
    */
    template <class ColIdList>
    concept ColIdListSpecifiable = mpl::has_value_parameters_v<ColIdList>
        && mpl::is_same_types_v<ColIdList>
        && mpl::is_unique_v<ColIdList>
        && mpl::Enumeration<mpl::get_front_t<ColIdList>>;

    /**
     * @class
     * @brief テーブルに主キー制約を指定する
     * @tparam ColIds 主キーとして設定するカラムIDを列挙する
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct primary_key : public constraint {};

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
    struct pk : public constraint_c {};

    /**
     * @brief 外部キー制約の正しい引数か検査
    */
    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = (sizeof...(ColIdLists) == 2)
        && (ColIdListSpecifiable<ColIdLists> && ...)
        && mpl::is_unique_v<mpl::concat_list_t<ColIdLists...>> // 二つのIDリストに一致するIDが含まれていてはならない
        && (mpl::count_v<ColIdLists> == ...);

    /**
     * @class
     * @brief テーブルに外部キー制約を指定する
     * @tparam ColIdList 外部キーとするIDの組
     * @tparam RefColIdList 外部キーとして参照するIDの組
    */
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};

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
    template <mpl::Enumeration auto RefColId> struct fk : public constraint_c {};

    /**
     * @brief カラムへ指定可能な制約のコンセプト
    */
    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    /**
     * @fn
     * @brief テーブルの制約か判定
    */
    template <class T> using is_constraint_definition = std::is_base_of<constraint, T>;

    /**
     * @brief テーブルの制約であること
    */
    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    /**
     * @brief テーブルへの制約リストであること
    */
    template <class T>
    concept ConstraintListDefinable = mpl::has_type_parameters_v<T>
        && mpl::is_unique_v<T>
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_constraint_definition>, T>>
        && mpl::count_if_v<mpl::quote<is_primary_key>, T> < 2; // 複数の主キーが一つのテーブルに存在してはいけない

    /**
     * @fn
     * @brief カラム定義用から通常のテーブル定義用に制約を変換する
     * @tparam Constraint カラムに指定している制約
     * @tparam ColId Constraintを指定している対象のカラムID
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId> struct to_table_constraint;

    // 各制約に対する特殊化
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};

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
    using constraint_unspecified = mpl::type_list<constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
