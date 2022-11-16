#include <iostream>
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

int main()
{
    constexpr auto concated_str = tudb::concat(tudb::cstr{"abudffjd"}, tudb::cstr{"abcd"}, tudb::cstr{"zyx"});
    constexpr auto converted_str = tudb::to_string<-255, 16, true>();
    std::cout << samples_def::name.buf << std::endl;
    constexpr auto id_info = samples_def::get_column_info<samples::ID>();
    constexpr auto name_info = samples_def::get_column_info<samples::NAME>();
    constexpr auto create_at_info = samples_def::get_column_info<samples::CREATE_AT>();
    std::cout << id_info.name.buf << ' '
        << name_info.name.buf << ' '
        << create_at_info.name.buf << ' '
        << concated_str.buf << converted_str.buf << std::endl;

    constexpr auto ff = tudb::cstr{"abc"} == tudb::cstr{"abcd"};
    std::cout << (ff ? "true" : "fals");

    using types1 = tudb::type_list<int, double, std::string>;
    using types2 = tudb::type_list<std::nullptr_t, std::size_t, char>;
    using types3 = tudb::type_list<long long>;
    using types4 = tudb::concat_type_list_t<types1, types2, types3>;

}
