#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, TypeListTest)
{
    // サイズ取得の確認
    constexpr auto case1 = tudb::type_list<int, float, double, char>::size;
    constexpr auto case2 = tudb::type_list<int>::size;
    // type_listであるか判定の確認
    constexpr auto case3 = tudb::is_type_list_v<int>;
    constexpr auto case4 = tudb::is_type_list_v<tudb::type_list<int, float, double, char>>;
    constexpr auto case5 = tudb::is_type_list_v<std::tuple<int, float, double, char>>;
    // pass_typesの確認
    using type5 = tudb::type_list<int, float, double, char>;
    using type1 = tudb::get_first_type_t<tudb::pass_types<std::tuple<int, float, double, char>>>;
    using type2 = tudb::get_first_type_t<int, float, double, char>;
    using type3 = tudb::get_first_type_t<tudb::pass_types<type5>>;
    using type4 = tudb::get_first_type_t<type5>;
    constexpr auto case6 = std::is_same_v<type1, type2>;
    constexpr auto case7 = std::is_same_v<type3, type2>;
    constexpr auto case8 = std::is_same_v<type3, type4>;
    constexpr auto case9 = std::is_same_v<type5, type4>;
    // copy_typesの確認
    using type6 = tudb::copy_types_t<type5, tudb::get_first_type_t>;
    constexpr auto case10 = std::is_same_v<type3, type6>;
    // has_type_parametersの確認
    constexpr auto case11 = tudb::has_type_parameters_v<std::vector<int>>;
    constexpr auto case12 = tudb::has_type_parameters_v<std::tuple<int, float, double, char>>;
    constexpr auto case13 = tudb::has_type_parameters_v<type5>;
    constexpr auto case14 = tudb::has_type_parameters_v<int>;
    // push_typeの確認
    constexpr auto case15 = std::is_same_v<tudb::push_type_t<int, type5>, tudb::type_list<int, int, float, double, char>>;
    constexpr auto case16 = std::is_same_v<tudb::push_type_t<type5, int>, tudb::type_list<int, float, double, char, int>>;
    // concat_type_listの確認
    using type7 = tudb::type_list<int, double, std::string>;
    using type8 = tudb::type_list<std::nullptr_t, std::size_t, char>;
    using type9 = tudb::concat_type_list_t<type7, type8, tudb::type_list<long long>>;
    constexpr auto case17 = std::is_same_v<type9, tudb::type_list<int, double, std::string, std::nullptr_t, std::size_t, char, long long>>;

    EXPECT_EQ(case1, 4);
    EXPECT_EQ(case2, 1);
    EXPECT_FALSE(case3);
    EXPECT_TRUE(case4);
    EXPECT_FALSE(case5);
    EXPECT_TRUE(case6);
    EXPECT_TRUE(case7);
    EXPECT_FALSE(case8);
    EXPECT_TRUE(case9);
    EXPECT_TRUE(case10);
    EXPECT_TRUE(case11);
    EXPECT_TRUE(case12);
    EXPECT_TRUE(case13);
    EXPECT_FALSE(case14);
    EXPECT_TRUE(case15);
    EXPECT_TRUE(case16);
    EXPECT_TRUE(case17);
}
