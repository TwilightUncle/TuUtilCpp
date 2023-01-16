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
    // constexpr auto case1 = db::get_type_name_string<db::bit>();
    // constexpr auto case2 = db::get_type_name_string<db::unsigned_bigint>();
    // constexpr auto case3 = db::get_type_name_string<db::varchar<0>>();
    // constexpr auto case4 = db::get_type_name_string<db::character<255>>();

    // EXPECT_STREQ(case1.data(), "bit");
    // EXPECT_STREQ(case2.data(), "bigint unsigned");
    // EXPECT_STREQ(case3.data(), "varchar(0)");
    // EXPECT_STREQ(case4.data(), "char(255)");

    db::validate_sql_identity<"_">();
    db::validate_sql_identity<"abcZYX109_">();
    db::validate_sql_identity<"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_a">();
    // 64�����I�[�o�[�ŃR���p�C���G���[
    // db::validate_sql_identity<"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ab">();
    // ���ʎq�Ƃ��ċ�����Ȃ��������܂܂�Ă��邽�߃R���p�C���G���[
    // db::validate_sql_identity<"abcZYX109_/">();
    // ���l����n�܂邽�߃R���p�C���G���[
    // db::validate_sql_identity<"1abcde">();
    // �󕶎��̂��߃R���p�C���G���[
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
            db::table_constraint,
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
    constexpr auto case18 = db::ConstraintListDefinable<
        mpl::type_list<
            db::auto_increment<samples::ID>,
            db::auto_increment<samples::ID2>, // ������auto_increment�������Ă͂����Ȃ�
            db::foreign_key<mpl::value_list<samples::ID, samples::ID2>, mpl::value_list<samples2::ID, samples2::ID2>>
        >
    >;
    constexpr auto case19 = db::ConstraintListDefinable<
        mpl::type_list<
            db::auto_increment<samples::ID>,    // ����Ŏw�肵�Ă���ColID�̌^�����ꂳ��Ă��Ȃ�����NG
            db::primary_key<samples2::ID, samples2::ID2>
        >
    >;

    EXPECT_TRUE(case13);
    EXPECT_TRUE(case14);
    EXPECT_FALSE(case15);
    EXPECT_FALSE(case16);
    EXPECT_FALSE(case17);
    EXPECT_FALSE(case18);
    EXPECT_FALSE(case19);

    constexpr auto case20 = std::is_same_v<db::to_table_constraint_t<db::pk, samples::ID>, db::primary_key<samples::ID>>;
    constexpr auto case21 = std::is_same_v<db::to_table_constraint_t<db::fk<samples2::ID>, samples::ID>, db::foreign_key<mpl::value_list<samples::ID>, mpl::value_list<samples2::ID>>>;

    EXPECT_TRUE(case20);
    EXPECT_TRUE(case21);
}

TEST(TuutilcppDbTest, DbColumnTest)
{
    using column_id = db::define_column<samples::ID, "id", db::integer, db::pk, db::ai, db::not_null>;
    using column_id2 = db::define_column<samples::ID2, "id2", db::integer, db::pk, db::fk<samples2::ID>>;
    using column_na = db::define_column<samples::NAME, "name", db::varchar<255>>;
    using column_ce = db::define_column<samples::CREATE_AT, "create_at", db::integer>;
    using column_definition_list = mpl::type_list<column_id, column_id2, column_na, column_ce>;

    // �����ȊO��ai���w�肵�Ă���̂ŃR���p�C���G���[
    // using bad_column_id = db::define_column<samples::ID, "id", db::varchar<255>, db::pk, db::ai>;

    // �J�����ł��邱�Ƃ̐���̃e�X�g(is_column_definition�����˂�)
    constexpr auto case1 = db::ColumnDefinable<column_id>;
    constexpr auto case2 = db::ColumnDefinable<column_na>;
    constexpr auto case3 = db::ColumnDefinable<column_ce>;
    constexpr auto case4 = db::ColumnDefinable<std::string>;
    // get_constraint_list�̃e�X�g
    constexpr auto case5 = std::is_same_v<db::get_constraint_list_t<int>, db::constraint_unspecified>; // define_column�ȊO
    constexpr auto case6 = std::is_same_v<
        db::get_constraint_list_t<column_id>,
        mpl::type_list<db::primary_key<samples::ID>, db::auto_increment<samples::ID>, db::table_constraint>
    >; // ������w�肳�ꂽ�J����
    constexpr auto case7 = std::is_same_v<
        db::get_constraint_list_t<column_id2>,
        mpl::type_list<
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >; // ����������w�肳�ꂽ�J����
    constexpr auto case8 = std::is_same_v<db::get_constraint_list_t<column_na>, db::constraint_unspecified>; // ���񖢎w��̃J����
    // �J������`���X�g�ł��邱��
    constexpr auto case9 = db::ColumnListDefinable<int>; // ���������p�����[�^�p�b�N�������Ă��Ȃ��^NG
    constexpr auto case10 = db::ColumnListDefinable<column_definition_list>; // OK
    constexpr auto case11 = db::ColumnListDefinable<mpl::type_list<column_id, column_na, int>>; // ���X�g����define_column�ȊO���܂�NG
    constexpr auto case12 = db::ColumnListDefinable<mpl::type_list<column_id, column_na, column_na>>; // ���X�g���̌^����ӂł͂Ȃ�NG
    constexpr auto case13 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples::ID, "bad_id", db::integer>>>; // ColID���d�����Ă���̂�NG
    constexpr auto case14 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples2::ID2, "id2", db::integer, db::pk>>>; // �قȂ�^��ColID���w�肳�ꂽ�J������`���܂܂��̂�NG
    constexpr auto case15 = db::ColumnListDefinable<mpl::type_list<column_id, db::define_column<samples::ID2, "id", db::integer>>>; // �J�������ɏd������NG
    // extract_constraints�e�X�g
    constexpr auto case16 = std::is_same_v<
        db::extract_constraints_t<column_definition_list>,
        mpl::type_list<
            db::primary_key<samples::ID>,
            db::auto_increment<samples::ID>,
            db::primary_key<samples::ID2>,
            db::foreign_key<mpl::value_list<samples::ID2>, mpl::value_list<samples2::ID>>
        >
    >;
    constexpr auto case17 = std::is_same_v<
        db::extract_constraints_t<mpl::type_list<column_na, column_ce>>,
        mpl::ignore_type
    >;
    // ��`���X�g�������̒�`���擾����
    constexpr auto case18 = std::is_same_v<db::get_column_def_t<samples::ID, column_definition_list>, column_id>;
    constexpr auto case19 = std::is_same_v<db::get_column_def_t<samples::ID2, column_definition_list>, column_id2>;
    constexpr auto case20 = std::is_same_v<db::get_column_def_t<samples::CREATE_AT, column_definition_list>, column_ce>;
    constexpr auto case21 = std::is_same_v<db::get_column_def_t<samples2::ID, column_definition_list>, mpl::ignore_type>;

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

TEST(TuutilcppDbTest, DbTableTest)
{
    using samples_def = db::define_table<
        samples,
        "samples",
        mpl::type_list<
            db::define_column<samples::ID, "id", db::integer, db::pk>,
            db::define_column<samples::ID2, "id2", db::integer, db::fk<samples2::ID>>,
            db::define_column<samples::NAME, "name", db::varchar<255>>,
            db::define_column<samples::CREATE_AT, "create_at", db::integer>
        >,
        mpl::type_list<
            db::unique<samples::ID2, samples::NAME>
        >
    >;

    // �e�[�u���Ŏw�肵��sample2�ƁA�J�����Ɏw�肵���񋓑̂̌^samples���قȂ��Ă��邽�߃R���p�C���G���[���N����
    // using error_samples_def = db::define_table<
    //     samples2,
    //     "samples",
    //     mpl::type_list<
    //         db::define_column<samples::ID, "id", db::integer, db::pk>,
    //         db::define_column<samples::NAME, "name", db::varchar<255>, db::fk<samples2::NAME>>
    //     >
    // >;

    // �e�[�u������Ƃ��Ē���auto_incriment���w�肵�Ă͂Ȃ�Ȃ�
    // using error_samples_def2 = db::define_table<
    //     samples,
    //     "samples",
    //     mpl::type_list<
    //         db::define_column<samples::ID, "id", db::integer, db::pk>,
    //         db::define_column<samples::NAME, "name", db::varchar<255>, db::fk<samples2::NAME>>
    //     >,
    //     mpl::type_list<
    //         db::auto_increment<samples::ID>
    //     >
    // >;

    constexpr auto case1 = db::get_column_def_t<samples::ID, samples_def>::name.data();
    constexpr auto case2 = db::get_column_def_t<samples::NAME, samples_def>::name.data();
    constexpr auto case3 = db::get_column_def_t<samples::CREATE_AT, samples_def>::name.data();

    EXPECT_STREQ(case1, "id");
    EXPECT_STREQ(case2, "name");
    EXPECT_STREQ(case3, "create_at");

    using samples2_def = db::define_table<
        samples2,
        "samples2",
        mpl::type_list<
            db::define_column<samples2::ID, "id", db::integer, db::pk>,
            db::define_column<samples2::NAME, "name", db::varchar<255>>,
            db::define_column<samples2::CREATE_AT, "create_at", db::integer>
        >
    >;
    constexpr auto case4 = db::TableListDefinable<mpl::type_list<samples_def, samples2_def>>;
    EXPECT_TRUE(case4);
    
    constexpr auto case5 = db::TableListDefinable<mpl::type_list<samples_def, samples_def>>;
    EXPECT_FALSE(case5);    // ���X�g���ɏd�����Ă���^�������Ă͂����Ȃ�

    constexpr auto case6 = db::TableListDefinable<mpl::type_list<samples_def, int>>;
    EXPECT_FALSE(case6); // ���X�g�Ƀe�[�u����`�ł͂Ȃ����̂��܂܂�Ă��邽��NG

    using samples2_same_id_def = db::define_table<
        samples,
        "samples2",
        mpl::type_list<
            db::define_column<samples::ID, "id", db::integer, db::pk>,
            db::define_column<samples::NAME, "name", db::varchar<255>>,
            db::define_column<samples::CREATE_AT, "create_at", db::integer>
        >
    >;
    constexpr auto case7 = db::TableListDefinable<mpl::type_list<samples_def, samples2_same_id_def>>;
    EXPECT_FALSE(case7);    // samples_def��ETableType�֎w�肵���^�����Ԃ��Ă���

    using samples2_same_name_def = db::define_table<
        samples2,
        "samples",
        mpl::type_list<
            db::define_column<samples2::ID, "id", db::integer, db::pk>,
            db::define_column<samples2::NAME, "name", db::varchar<255>>,
            db::define_column<samples2::CREATE_AT, "create_at", db::integer>
        >
    >;
    constexpr auto case8 = db::TableListDefinable<mpl::type_list<samples_def, samples2_same_name_def>>;
    EXPECT_FALSE(case8);    // samples_def��Name�Ɏw�肵�������񂪂��Ԃ��Ă��邽��NG

    constexpr auto case9 = std::is_same_v<db::get_table_def_t<samples, mpl::type_list<samples_def, samples2_def>>, samples_def>;
    constexpr auto case10 = std::is_same_v<db::get_table_def_t<samples2, mpl::type_list<samples_def, samples2_def>>, samples2_def>;

    EXPECT_TRUE(case9);
    EXPECT_TRUE(case10);
}
