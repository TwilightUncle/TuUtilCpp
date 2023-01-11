#include <gtest/gtest.h>
#include <tudbcpp/type.hpp>
#include <tudbcpp/column.hpp>

enum class samples {
    ID, ID2, NAME, CREATE_AT
};
enum class samples2 {
    ID,
};

TEST(tudbcpptest, DbColumnTest)
{
    using column_id = tudb::define_column<samples::ID, "id", tudb::integer, tudb::pk>;
    using column_id2 = tudb::define_column<samples::ID2, "id2", tudb::integer, tudb::pk, tudb::fk<samples2::ID>>;
    using column_na = tudb::define_column<samples::NAME, "name", tudb::varchar<256>>;
    using column_ce = tudb::define_column<samples::CREATE_AT, "create_at", tudb::integer>;
    using column_definition_list = tuutil::mpl::type_list<column_id, column_id2, column_na, column_ce>;

    // カラムであることの制約のテスト(is_column_definitionも兼ねる)
    constexpr auto case1 = tudb::ColumnDefinable<column_id>;
    constexpr auto case2 = tudb::ColumnDefinable<column_na>;
    constexpr auto case3 = tudb::ColumnDefinable<column_ce>;
    constexpr auto case4 = tudb::ColumnDefinable<std::string>;
    // get_constraint_listのテスト
    constexpr auto case5 = std::is_same_v<tudb::get_constraint_list_t<int>, tudb::constraint_unspecified>; // define_column以外
    constexpr auto case6 = std::is_same_v<tudb::get_constraint_list_t<column_id>, tuutil::mpl::type_list<tudb::primary_key<samples::ID>>>; // 制約を指定されたカラム
    constexpr auto case7 = std::is_same_v<
        tudb::get_constraint_list_t<column_id2>,
        tuutil::mpl::type_list<
            tudb::primary_key<samples::ID2>,
            tudb::foreign_key<tuutil::mpl::value_list<samples::ID2>, tuutil::mpl::value_list<samples2::ID>>
        >
    >; // 複数制約を指定されたカラム
    constexpr auto case8 = std::is_same_v<tudb::get_constraint_list_t<column_na>, tudb::constraint_unspecified>; // 制約未指定のカラム
    // カラム定義リストであること
    constexpr auto case9 = tudb::ColumnListDefinitionable<int>; // そもそもパラメータパックを持っていない型NG
    constexpr auto case10 = tudb::ColumnListDefinitionable<column_definition_list>; // OK
    constexpr auto case11 = tudb::ColumnListDefinitionable<tuutil::mpl::type_list<column_id, column_na, int>>; // リスト内にdefine_column以外を含むNG
    constexpr auto case12 = tudb::ColumnListDefinitionable<tuutil::mpl::type_list<column_id, column_na, column_na>>; // リスト内の型が一意ではないNG
    // extract_constraintsテスト
    constexpr auto case13 = std::is_same_v<
        tudb::extract_constraints_t<column_definition_list>,
        tuutil::mpl::type_list<
            tudb::primary_key<samples::ID>,
            tudb::primary_key<samples::ID2>,
            tudb::foreign_key<tuutil::mpl::value_list<samples::ID2>, tuutil::mpl::value_list<samples2::ID>>
        >
    >;
    constexpr auto case14 = std::is_same_v<
        tudb::extract_constraints_t<tuutil::mpl::type_list<column_na, column_ce>>,
        tuutil::mpl::ignore_type
    >;

    EXPECT_TRUE(case1);
    EXPECT_TRUE(case2);
    EXPECT_TRUE(case3);
    EXPECT_FALSE(case4);
    EXPECT_TRUE(case5);
    EXPECT_TRUE(case6);
    EXPECT_TRUE(case7);
    EXPECT_TRUE(case8);
    EXPECT_FALSE(case9);
    EXPECT_TRUE(case10);
    EXPECT_FALSE(case11);
    EXPECT_FALSE(case12);
    EXPECT_TRUE(case13);
    EXPECT_TRUE(case14);
}
