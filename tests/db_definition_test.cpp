#include <gtest/gtest.h>
#include <tudbcpp/type.hpp>
#include <tudbcpp/table.hpp>

enum class samples {
    ID, NAME, CREATE_AT
};
enum class samples2 {
    ID, NAME, CREATE_AT
};

using samples_def = tudb::define_table<
    samples,
    "samples",
    tuutil::mpl::type_list<
        tudb::define_column<samples::ID, "id", tudb::integer, tudb::pk>,
        tudb::define_column<samples::NAME, "name", tudb::varchar<256>, tudb::fk<samples2::NAME>>,
        tudb::define_column<samples::CREATE_AT, "create_at", tudb::integer>
    >
>;

TEST(tudbcpptest, DbDefinitionTest)
{
    EXPECT_STREQ(samples_def::get_column_info<samples::ID>().name.data(), "id");
    EXPECT_STREQ(samples_def::get_column_info<samples::NAME>().name.data(), "name");
    EXPECT_STREQ(samples_def::get_column_info<samples::CREATE_AT>().name.data(), "create_at");
}
