#include <gtest/gtest.h>
#include <tuutilcpp/db.hpp>

using namespace tuutil;
using namespace tuutil::db;

enum class samples {
    ID, ID2, NAME, CREATE_AT
};
enum class samples2 {
    ID,
};

TEST(TuutilcppDbTest, DbColumnTest)
{
    using column_id = define_column<samples::ID, "id", integer, pk>;
    using column_id2 = define_column<samples::ID2, "id2", integer, pk, fk<samples2::ID>>;
    using column_na = define_column<samples::NAME, "name", varchar<256>>;
    using column_ce = define_column<samples::CREATE_AT, "create_at", integer>;
    using column_definition_list = mpl::type_list<column_id, column_id2, column_na, column_ce>;

    // カラムであることの制約のテスト(is_column_definitionも兼ねる)
    constexpr auto case1 = ColumnDefinable<column_id>;
    constexpr auto case2 = ColumnDefinable<column_na>;
    constexpr auto case3 = ColumnDefinable<column_ce>;
    constexpr auto case4 = ColumnDefinable<std::string>;
    // get_constraint_listのテスト
    constexpr auto case5 = std::is_same_v<get_constraint_list_t<int>, constraint_unspecified>; // define_column以外
    constexpr auto case6 = std::is_same_v<get_constraint_list_t<column_id>, mpl::type_list<primary_key<samples::ID>>>; // 制約を指定されたカラム
    constexpr auto case7 = std::is_same_v<
        get_constraint_list_t<column_id2>,
        mpl::type_list<
            primary_key<samples::ID2>,
            foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >; // 複数制約を指定されたカラム
    constexpr auto case8 = std::is_same_v<get_constraint_list_t<column_na>, constraint_unspecified>; // 制約未指定のカラム
    // カラム定義リストであること
    constexpr auto case9 = ColumnListDefinitionable<int>; // そもそもパラメータパックを持っていない型NG
    constexpr auto case10 = ColumnListDefinitionable<column_definition_list>; // OK
    constexpr auto case11 = ColumnListDefinitionable<mpl::type_list<column_id, column_na, int>>; // リスト内にdefine_column以外を含むNG
    constexpr auto case12 = ColumnListDefinitionable<mpl::type_list<column_id, column_na, column_na>>; // リスト内の型が一意ではないNG
    // extract_constraintsテスト
    constexpr auto case13 = std::is_same_v<
        extract_constraints_t<column_definition_list>,
        mpl::type_list<
            primary_key<samples::ID>,
            primary_key<samples::ID2>,
            foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >;
    constexpr auto case14 = std::is_same_v<
        extract_constraints_t<mpl::type_list<column_na, column_ce>>,
        mpl::ignore_type
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
