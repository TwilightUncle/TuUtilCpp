#include <gtest/gtest.h>
#include <type.hpp>
#include <table.hpp>

enum class samples {
    ID, NAME, CREATE_AT
};
enum class samples2 {
    ID, NAME, CREATE_AT
};

using samples_def = tudb::define_table<
    samples,
    tudb::cstr{"samples"},
    tudb::type_list<
        tudb::define_column<samples::ID, tudb::cstr{"id"}, tudb::integer, tudb::pk>,
        tudb::define_column<samples::NAME, tudb::cstr{"name"}, tudb::varchar<256>, tudb::fk<samples2::NAME>>,
        tudb::define_column<samples::CREATE_AT, tudb::cstr{"create_at"}, tudb::integer>
    >
>;

TEST(tudbcpptest, DbDefinitionTest)
{
    EXPECT_STREQ(samples_def::get_column_info<samples::ID>().name.data(), "id");
    EXPECT_STREQ(samples_def::get_column_info<samples::NAME>().name.data(), "name");
    EXPECT_STREQ(samples_def::get_column_info<samples::CREATE_AT>().name.data(), "create_at");
}
