#include <gtest/gtest.h>
#include <tuutilcpp/db.hpp>

using namespace tuutil;

enum class samples {
    ID, ID2, NAME, CREATE_AT
};
enum class samples2 {
    ID, ID2, NAME, CREATE_AT
};

TEST(TuutilcppDbTest, TypeTest)
{
    db::validate_sql_identity<"_">();
    db::validate_sql_identity<"abcZYX109_">();
    db::validate_sql_identity<"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_a">();
    // 64文字オーバーでコンパイルエラー
    // db::validate_sql_identity<"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ab">();
    // 識別子として許可されない文字が含まれているためコンパイルエラー
    // db::validate_sql_identity<"abcZYX109_/">();
    // 数値から始まるためコンパイルエラー
    // db::validate_sql_identity<"1abcde">();
    // 空文字のためコンパイルエラー
    // db::validate_sql_identity<"">();
}

TEST(TuutilcppDbTest, ConstraintTest)
{
    constexpr auto case1 = db::ColIdListSpecifiable<mpl::value_list<samples::ID, samples::ID2>>;
    constexpr auto case2 = db::ColIdListSpecifiable<mpl::value_list<samples::ID, samples2::ID>>;
    constexpr auto case3 = db::ColIdListSpecifiable<mpl::value_list<samples::ID, samples::ID>>;
    constexpr auto case4 = db::ColIdListSpecifiable<mpl::value_list<int(2), int(1)>>;

    EXPECT_TRUE(case1);
    EXPECT_FALSE(case2);
    EXPECT_FALSE(case3);
    EXPECT_FALSE(case4);

    constexpr auto case5 = db::is_primary_key_v<db::primary_key<samples::ID, samples::ID2>>;
    constexpr auto case6 = db::is_primary_key_v<int>;

    EXPECT_TRUE(case5);
    EXPECT_FALSE(case6);

    constexpr auto case7 = db::ForeignKeyArgsSpecifiable<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>;
    constexpr auto case8 = db::ForeignKeyArgsSpecifiable<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples::NAME, samples::CREATE_AT>>;   // 自身のテーブルの別カラムの組もOK
    constexpr auto case9 = db::ForeignKeyArgsSpecifiable<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples::ID, samples::NAME>>;
    constexpr auto case10 = db::ForeignKeyArgsSpecifiable<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2, samples2::NAME>>;

    EXPECT_TRUE(case7);
    EXPECT_TRUE(case8);
    EXPECT_FALSE(case9);
    EXPECT_FALSE(case10);

    constexpr auto case11 = db::is_foreign_key_v<db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>>;
    constexpr auto case12 = db::is_foreign_key_v<int>;
    
    EXPECT_TRUE(case11);
    EXPECT_FALSE(case12);

    constexpr auto case13 = db::ConstraintListDefinable<
        mpl::type_list<
            db::constraint,
            db::primary_key<samples::ID, samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;
    constexpr auto case14 = db::ConstraintListDefinable<
        mpl::type_list<
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;
    constexpr auto case15 = db::ConstraintListDefinable<
        mpl::type_list<
            db::primary_key<samples::ID, samples::ID2>,
            db::primary_key<samples::NAME, samples::CREATE_AT>, // 複数の主キーがあってはならない
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;
    constexpr auto case16 = db::ConstraintListDefinable<
        mpl::type_list<
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>,
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;
    constexpr auto case17 = db::ConstraintListDefinable<
        mpl::type_list<
            int, // 制約以外の型
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;

    EXPECT_TRUE(case13);
    EXPECT_TRUE(case14);
    EXPECT_FALSE(case15);
    EXPECT_FALSE(case16);
    EXPECT_FALSE(case17);

    constexpr auto case18 = std::is_same_v<db::to_table_constraint_t<db::pk, samples::ID>, db::primary_key<samples::ID>>;
    constexpr auto case19 = std::is_same_v<db::to_table_constraint_t<db::fk<samples2::ID>, samples::ID>, db::foreign_key<mpl::value_list<samples::ID>, mpl::value_list<samples2::ID>>>;

    EXPECT_TRUE(case18);
    EXPECT_TRUE(case19);
}

TEST(TuutilcppDbTest, DbColumnTest)
{
    using column_id = db::define_column<samples::ID, "id", db::integer, db::pk>;
    using column_id2 = db::define_column<samples::ID2, "id2", db::integer, db::pk, db::fk<samples2::ID>>;
    using column_na = db::define_column<samples::NAME, "name", db::varchar<256>>;
    using column_ce = db::define_column<samples::CREATE_AT, "create_at", db::integer>;
    using column_definition_list = mpl::type_list<column_id, column_id2, column_na, column_ce>;

    // カラムであることの制約のテスト(is_column_definitionも兼ねる)
    constexpr auto case1 = db::ColumnDefinable<column_id>;
    constexpr auto case2 = db::ColumnDefinable<column_na>;
    constexpr auto case3 = db::ColumnDefinable<column_ce>;
    constexpr auto case4 = db::ColumnDefinable<std::string>;
    // get_constraint_listのテスト
    constexpr auto case5 = std::is_same_v<db::get_constraint_list_t<int>, db::constraint_unspecified>; // define_column以外
    constexpr auto case6 = std::is_same_v<db::get_constraint_list_t<column_id>, mpl::type_list<db::primary_key<samples::ID>>>; // 制約を指定されたカラム
    constexpr auto case7 = std::is_same_v<
        db::get_constraint_list_t<column_id2>,
        mpl::type_list<
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >; // 複数制約を指定されたカラム
    constexpr auto case8 = std::is_same_v<db::get_constraint_list_t<column_na>, db::constraint_unspecified>; // 制約未指定のカラム
    // カラム定義リストであること
    constexpr auto case9 = db::ColumnListDefinable<int>; // そもそもパラメータパックを持っていない型NG
    constexpr auto case10 = db::ColumnListDefinable<column_definition_list>; // OK
    constexpr auto case11 = db::ColumnListDefinable<mpl::type_list<column_id, column_na, int>>; // リスト内にdefine_column以外を含むNG
    constexpr auto case12 = db::ColumnListDefinable<mpl::type_list<column_id, column_na, column_na>>; // リスト内の型が一意ではないNG
    constexpr auto case13 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples::ID, "bad_id", db::integer>>>; // ColIDが重複しているのでNG
    constexpr auto case14 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples2::ID2, "id2", db::integer, db::pk>>>; // 異なる型のColIDが指定されたカラム定義が含まれるのでNG
    constexpr auto case15 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples::ID2, "id", db::integer>>>; // カラム名に重複ありNG
    // extract_constraintsテスト
    constexpr auto case16 = std::is_same_v<
        db::extract_constraints_t<column_definition_list>,
        mpl::type_list<
            db::primary_key<samples::ID>,
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >;
    constexpr auto case17 = std::is_same_v<
        db::extract_constraints_t<mpl::type_list<column_na, column_ce>>,
        mpl::ignore_type
    >;
    // 定義リストから特定の定義を取得する
    constexpr auto case18 = std::is_same_v<db::get_column_def<samples::ID, column_definition_list>, column_id>;
    constexpr auto case19 = std::is_same_v<db::get_column_def<samples::ID2, column_definition_list>, column_id2>;
    constexpr auto case20 = std::is_same_v<db::get_column_def<samples::CREATE_AT, column_definition_list>, column_ce>;
    constexpr auto case21 = std::is_same_v<db::get_column_def<samples2::ID, column_definition_list>, mpl::ignore_type>;

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
    EXPECT_FALSE(case13);
    EXPECT_FALSE(case14);
    EXPECT_FALSE(case15);
    EXPECT_TRUE(case16);
    EXPECT_TRUE(case17);
    EXPECT_TRUE(case18);
    EXPECT_TRUE(case19);
    EXPECT_TRUE(case20);
    EXPECT_TRUE(case21);
}

TEST(TuutilcppDbTest, DbDefinitionTest)
{
    using samples_def = db::define_table<
        samples,
        "samples",
        mpl::type_list<
            db::define_column<samples::ID, "id", db::integer, db::pk>,
            db::define_column<samples::NAME, "name", db::varchar<256>, db::fk<samples2::NAME>>,
            db::define_column<samples::CREATE_AT, "create_at", db::integer>
        >
    >;

    constexpr auto case1 = db::get_column_def<samples::ID, samples_def>::name.data();
    constexpr auto case2 = db::get_column_def<samples::NAME, samples_def>::name.data();
    constexpr auto case3 = db::get_column_def<samples::CREATE_AT, samples_def>::name.data();

    EXPECT_STREQ(case1, "id");
    EXPECT_STREQ(case2, "name");
    EXPECT_STREQ(case3, "create_at");
}
