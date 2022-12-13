#include <gtest/gtest.h>
#include <tuutilcpp/utility.hpp>

TEST(tudbcpptest, ValueTypePairTest)
{
    using type1 = tudb::value_type_pair<1, int>;
    constexpr auto case1 = tudb::is_value_type_pair<type1>::value;
    constexpr auto case2 = tudb::is_value_type_pair<int>::value;
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);

    // 連想配列
    using type2 = tudb::type_list<
        tudb::value_type_pair<1, int>,
        tudb::value_type_pair<2, double>,
        tudb::value_type_pair<3.f, std::string>
    >;
    // 一つでも連想配列の要件に見合わない要素が含まれている
    using type3 = tudb::type_list<
        tudb::value_type_pair<1, int>,
        tudb::value_type_pair<2, double>,
        tudb::value_type_pair<3.f, std::string>,
        tudb::value_list<4, 5>
    >;

    constexpr auto case3 = tudb::AssociativeTypeList<type2>;
    constexpr auto case4 = tudb::AssociativeTypeList<type3>;

    ASSERT_TRUE(case3);
    ASSERT_FALSE(case4);

    constexpr auto case5 = std::is_same_v<tudb::get_t<3.f, type2>, std::string>;
    constexpr auto case6 = std::is_same_v<tudb::get_t<1, type2>, int>;

    EXPECT_TRUE(case5);
    EXPECT_TRUE(case6);
}
