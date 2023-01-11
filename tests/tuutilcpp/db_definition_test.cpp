#include <gtest/gtest.h>
#include <tuutilcpp/db.hpp>

using namespace tuutil;
using namespace tuutil::db;

enum class samples {
    ID, NAME, CREATE_AT
};
enum class samples2 {
    ID, NAME, CREATE_AT
};

using samples_def = define_table<
    samples,
    "samples",
    mpl::type_list<
        define_column<samples::ID, "id", integer, pk>,
        define_column<samples::NAME, "name", varchar<256>, fk<samples2::NAME>>,
        define_column<samples::CREATE_AT, "create_at", integer>
    >
>;

TEST(TuutilcppDbTest, DbDefinitionTest)
{
    EXPECT_STREQ(samples_def::get_column_info<samples::ID>().name.data(), "id");
    EXPECT_STREQ(samples_def::get_column_info<samples::NAME>().name.data(), "name");
    EXPECT_STREQ(samples_def::get_column_info<samples::CREATE_AT>().name.data(), "create_at");
}
