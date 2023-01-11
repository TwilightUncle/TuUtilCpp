///----------------------------------
/// @file column.hpp
/// @brief DB列定義
///----------------------------------
#ifndef TUDBCPP_INCLUDE_GUARD_COLUMN_HPP
#define TUDBCPP_INCLUDE_GUARD_COLUMN_HPP

#include <tudbcpp/constraint.hpp>
#include <tudbcpp/type.hpp>
#include <tustrcpp/cstr.hpp>

namespace tudb
{
    /**
     * @fn
     * @brief 列定義用メタ関数(同時に定義内容参照用クラス)
     * @tparam ColID スコープ付き列挙体要素を指定
     * @tparam Name tustr::cstrで包んだ文字列リテラルを渡す。文字列は1文字以上
     * @tparam FieldType 格納するデータ型
     * @tparam Constraints 列制約を任意数指定する(指定なしもOK)
    */
    template <
        mpl::Enumeration auto ColID,
        tustr::cstr Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    requires (Name.size() > 0)
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
    template <mpl::Enumeration auto ColID, tustr::cstr Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_columnであることを強制する制約
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    template <ColumnDefinable T>
    struct get_column_id { static constexpr auto value = T::id; };

    template <ColumnDefinable T>
    struct get_column_name { static constexpr auto value = T::name; };

    /**
     * @fn
     * @brief define_columnで指定した制約を取り出す。ほかの任意の型を渡した場合は、constraint_unspecifiedを返す
    */
    template <class T> struct get_constraint_list : public std::type_identity<constraint_unspecified>{};
    template <class T>
    requires ColumnDefinable<T>
    struct get_constraint_list<T> : public std::type_identity<typename T::constraint_list> {};

    template <class T> using get_constraint_list_t = get_constraint_list<T>::type;

    /**
     * カラム定義リストであること
    */
    template <class T>
    concept ColumnListDefinitionable = requires {
        // Tはパラメータパックを持っている
        requires mpl::has_type_parameters_v<T>;

        // 同じ定義のカラムが存在してはいけない
        requires mpl::is_unique_v<T>;

        // テンプレート引数が全てカラム定義であること(全てのパラメータがカラム定義であるかをリストとして取得し、論理積で結果を確認)
        requires mpl::apply_list<
            mpl::quote<std::conjunction>,
            mpl::map_t<mpl::quote<is_column_definition>, T>
        >::value;

        // nameがかぶってはいけない
        // idがかぶってはいけない
        // idの型は全て等しい必要がある
    };

    /**
     * @fn
     * @brief カラムの定義リストから、各カラムに指定された制約情報を抽出する(一つもなければtype_list<ignore_type>が返る)
    */
    template <ColumnListDefinitionable T>
    struct extract_constraints : public mpl::relay<
        T,
        mpl::type_list<
            mpl::bind<mpl::quote<mpl::map>, mpl::quote<get_constraint_list>>,
            mpl::bind<mpl::quote<mpl::apply_list>, mpl::quote<mpl::concat_type_list>>,
            mpl::bind<mpl::quote<mpl::remove_if>, mpl::bind<mpl::quote<std::is_same>, constraint>>
        >
    > {};

    template <ColumnListDefinitionable T> using extract_constraints_t = extract_constraints<T>::type;
}

#endif // TUDBCPP_INCLUDE_GUARD_COLUMN_HPP
