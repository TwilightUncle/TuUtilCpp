#include <gtest/gtest.h>
#include <tuutilcpp/db.hpp>

using namespace tuutil;

enum class samples {
    ID, ID2, NAME, CREATE_AT
};
enum class samples2 {
    ID, ID2, NAME, CREATE_AT
};

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
    constexpr auto case8 = db::ForeignKeyArgsSpecifiable<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples::NAME, samples::CREATE_AT>>;   // ���g�̃e�[�u���̕ʃJ�����̑g��OK
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
            db::primary_key<samples::NAME, samples::CREATE_AT>, // �����̎�L�[�������Ă͂Ȃ�Ȃ�
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
            int, // ����ȊO�̌^
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

    // �J�����ł��邱�Ƃ̐���̃e�X�g(is_column_definition�����˂�)
    constexpr auto case1 = db::ColumnDefinable<column_id>;
    constexpr auto case2 = db::ColumnDefinable<column_na>;
    constexpr auto case3 = db::ColumnDefinable<column_ce>;
    constexpr auto case4 = db::ColumnDefinable<std::string>;
    // get_constraint_list�̃e�X�g
    constexpr auto case5 = std::is_same_v<db::get_constraint_list_t<int>, db::constraint_unspecified>; // define_column�ȊO
    constexpr auto case6 = std::is_same_v<db::get_constraint_list_t<column_id>, mpl::type_list<db::primary_key<samples::ID>>>; // ������w�肳�ꂽ�J����
    constexpr auto case7 = std::is_same_v<
        db::get_constraint_list_t<column_id2>,
        mpl::type_list<
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >; // ����������w�肳�ꂽ�J����
    constexpr auto case8 = std::is_same_v<db::get_constraint_list_t<column_na>, db::constraint_unspecified>; // ���񖢎w��̃J����
    // �J������`���X�g�ł��邱��
    constexpr auto case9 = db::ColumnListDefinitionable<int>; // ���������p�����[�^�p�b�N�������Ă��Ȃ��^NG
    constexpr auto case10 = db::ColumnListDefinitionable<column_definition_list>; // OK
    constexpr auto case11 = db::ColumnListDefinitionable<mpl::type_list<column_id, column_na, int>>; // ���X�g����define_column�ȊO���܂�NG
    constexpr auto case12 = db::ColumnListDefinitionable<mpl::type_list<column_id, column_na, column_na>>; // ���X�g���̌^����ӂł͂Ȃ�NG
    // extract_constraints�e�X�g
    constexpr auto case13 = std::is_same_v<
        db::extract_constraints_t<column_definition_list>,
        mpl::type_list<
            db::primary_key<samples::ID>,
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >;
    constexpr auto case14 = std::is_same_v<
        db::extract_constraints_t<mpl::type_list<column_na, column_ce>>,
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

    EXPECT_STREQ(samples_def::get_column_info<samples::ID>().name.data(), "id");
    EXPECT_STREQ(samples_def::get_column_info<samples::NAME>().name.data(), "name");
    EXPECT_STREQ(samples_def::get_column_info<samples::CREATE_AT>().name.data(), "create_at");
}
