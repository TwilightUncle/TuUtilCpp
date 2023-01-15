///----------------------------------
/// @file column.hpp
/// @brief DB列定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP

namespace tuutil::db
{
    /**
     * @fn
     * @brief 列定義用メタ関数(同時に定義内容参照用クラス)
     * @tparam ColID スコープ付き列挙体要素を指定
     * @tparam Name str::cstrで包んだ文字列リテラルを渡す。文字列は1文字以上
     * @tparam FieldType 格納するデータ型
     * @tparam Constraints 列制約を任意数指定する(指定なしもOK)
    */
    template <
        mpl::Enumeration auto ColID,
        str::cstr Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    requires (validate_sql_identity<Name>())
    struct define_column
    {
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;
        using constraint_list = std::conditional_t<
            (sizeof...(Constraints) > 0),
            mpl::type_list<to_table_constraint_t<Constraints, ColID>...>,
            constraint_unspecified
        >;
    };

    /**
     * @fn
     * @brief column_definition型かどうかを判定するメタ関数
    */
    template <class T> struct is_column_definition : public std::false_type {};
    template <mpl::Enumeration auto ColID, str::cstr Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_columnであることを強制する制約
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    /**
     * @fn
     * @brief カラム定義からカラムIDを示す列挙体の特性クラスを取得する
    */
    template <ColumnDefinable T>
    struct get_column_id : public std::type_identity<mpl::value_constant<T::id>> {};

    /**
     * @fn
     * @brief カラム定義からカラムIDを示す列挙体の型を取得
    */
    template <class T> using get_column_id_t = get_column_id<T>::type;

    /**
     * @fn
     * @brief カラム定義からカラムIDを示す列挙体の値を取得
    */
    template <class T> constexpr auto get_column_id_v = get_column_id<T>::value;

    /**
     * @fn
     * @brief カラム定義からカラム名の特性クラスを取得
    */
    template <ColumnDefinable T>
    struct get_column_name : public std::type_identity<mpl::value_constant<T::name>> {};

    /**
     * @fn
     * @brief カラム定義からカラム名の値を取得
    */
    template <class T> constexpr auto get_column_name_v = get_column_name<T>::value;

    /**
     * @fn
     * @brief define_columnで指定した制約を取り出す。ほかの任意の型を渡した場合は、constraint_unspecifiedを返す
    */
    template <class T> struct get_constraint_list : public std::type_identity<constraint_unspecified>{};
    template <class T>
    requires ColumnDefinable<T>
    struct get_constraint_list<T> : public std::type_identity<typename T::constraint_list> {};

    /**
     * @fn
     * @brief define_columnで指定した制約を取り出す。ほかの任意の型を渡した場合は、constraint_unspecifiedを返す
    */
    template <class T> using get_constraint_list_t = get_constraint_list<T>::type;

    /**
     * カラム定義リストであること
    */
    template <class T>
    concept ColumnListDefinable = mpl::has_type_parameters_v<T>                                             // Tは型のパラメータパックを持つこと
        && mpl::is_unique_v<T>                                                                              // Tが持つパラメータパックは重複がないこと
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_column_definition>, T>> // Tが持つパラメータパックは全てcolumn_definitionであること
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_column_id>, T>>                                       // Tが持つパラメータパック要素のメンバidは全て異なる値であること
        && mpl::is_same_types_v<mpl::map_t<mpl::quote<get_column_id_t>, T>>                                 // Tが持つパラメータパック要素のメンバidは全て同じ型であること
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_column_name>, T>>;                                    // Tが持つパラメータパック要素のメンバnameは全て異なる値であること

    /**
     * @fn
     * @brief カラムの定義リストから、各カラムに指定された制約情報を抽出する(一つもなければtype_list<ignore_type>が返る)
    */
    template <ColumnListDefinable T>
    struct extract_constraints : public mpl::relay<
        T,
        mpl::type_list<
            mpl::bind<mpl::quote<mpl::map>, mpl::quote<get_constraint_list>>,
            mpl::bind<mpl::quote<mpl::apply_list>, mpl::quote<mpl::concat_list>>,
            mpl::bind<mpl::quote<mpl::remove_if>, mpl::bind<mpl::quote<std::is_same>, constraint>>
        >
    > {};

    template <ColumnListDefinable T> using extract_constraints_t = extract_constraints<T>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP
