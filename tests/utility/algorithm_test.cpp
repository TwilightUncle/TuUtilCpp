#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, GetFirstTypeTest)
{
    // リストのうち最初の型を返す
    constexpr auto case1 = std::is_same_v<tudb::get_first_type_t<int, void, double>, int>;
    constexpr auto case2 = std::is_same_v<tudb::get_first_type_t<void, double, int>, int>;
    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
}

// テスト用メタ関数定義(受け取った二つの引数の内型のサイズが大きいほうを返す)
template <class T1, class T2> struct get_max_size_type : std::conditional<(sizeof(T1) > sizeof(T2)), T1, T2> {};
template <class T1, class T2> struct get_min_size_type : std::conditional<(sizeof(T1) <= sizeof(T2)), T1, T2> {};

TEST(tudbcpptest, FoldByTypeTest)
{
    // 左畳み込み
    using l_max = tudb::foldl_by_type_t<get_max_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    using l_min = tudb::foldl_by_type_t<get_min_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    // 右畳み込み
    using r_max = tudb::foldr_by_type_t<get_max_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;
    using r_min = tudb::foldr_by_type_t<get_min_size_type, long, long long, short, unsigned short, unsigned long long, unsigned long>;

    constexpr auto case1 = std::is_same_v<l_max, unsigned long long>;
    constexpr auto case2 = std::is_same_v<l_min, short>;
    constexpr auto case3 = std::is_same_v<r_max, long long>;
    constexpr auto case4 = std::is_same_v<r_min, unsigned short>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}

// どの型を渡してもintを返す
template <class T> struct to_int : std::type_identity<int> {};

TEST(tudbcpptest, MapTypesTest)
{
    // メタ関数が全ての型に適用されていること
    using maped = tudb::map_types_t<to_int, void, double, int, char>;
    constexpr auto case1 = std::is_same_v<maped, tudb::type_list<int, int, int, int>>;
    ASSERT_TRUE(case1);
}

template <int V, class T> struct test_same_value : std::bool_constant<V == T::value> {};

TEST(tudbcpptest, FindIfTypeTest)
{
    // メタ関数が真を返すものが含まれていた場合、最初に真を返す型を返却し、なければvoidを返却
    using find_type_int = tudb::find_if_by_type_t<int, std::is_same, double, long long, char, int>;
    using not_find_type1 = tudb::find_if_by_type_t<short, std::is_same, double, long long, char, int>;
    using find_type_1 = tudb::find_if_by_value_t<1, test_same_value, std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2>>;
    using not_find_type2 = tudb::find_if_by_value_t<5, test_same_value, std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2>>;

    constexpr auto case1 = std::is_same_v<find_type_int, int>;
    constexpr auto case2 = std::is_same_v<not_find_type1, void>;
    constexpr auto case3 = std::is_same_v<find_type_1, std::integral_constant<int, 1>>;
    constexpr auto case4 = std::is_same_v<not_find_type2, void>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}

TEST(tudbcpptest, EqualValuesTest)
{
    // 型と値がともに等しい場合のみ真となることを確認
    constexpr auto case1 = tudb::equal_values_v<(int)5, (int)5>;
    constexpr auto case2 = tudb::equal_values_v<(int)5, (int)4>;
    constexpr auto case3 = tudb::equal_values_v<(char)5, (long)5>;
    constexpr auto case4 = tudb::equal_values_v<(char)5, (long)4>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IncludeTest)
{
    // 検査対象のリストに型が等しいものが存在する場合真
    constexpr auto case1 = tudb::include_type_v<int, double, void, int, long long>;
    constexpr auto case2 = tudb::include_type_v<int, double, void, long, long long>;
    // 検査対象のリストに型と値がともに等しいものが存在する場合真
    constexpr auto case3 = tudb::include_value_v<(int)1, (char)1, (long)2, (int)1, (long long)0>;
    constexpr auto case4 = tudb::include_value_v<(int)1, (char)1, (long)2, (unsigned int)1, (long long)0>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IsUniqueTest)
{
    // 一つでも一致するものが含まれていたら偽
    constexpr auto case1 = tudb::is_unique_v<int, void, short, long>;
    constexpr auto case2 = tudb::is_unique_v<int, void, short, void>;
    constexpr auto case3 = tudb::is_unique_values_v<(int)1, (int)2, (short)1, (short)2>;
    constexpr auto case4 = tudb::is_unique_values_v<(int)1, (int)2, (short)1, (int)2>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_FALSE(case4);
}

TEST(tudbcpptest, IsSameTest)
{
    // 一つでも異なるものが含まれていたら偽
    constexpr auto case1 = tudb::is_same_types_v<int, int, int, int>;
    constexpr auto case2 = tudb::is_same_types_v<int, int, char, int>;
    // 一つでも型が異なるものが含まれていたら偽(値は異なってもOK)
    constexpr auto case3 = tudb::is_same_value_types_v<(int)1, (int)1, (int)1, (int)1>;
    constexpr auto case4 = tudb::is_same_value_types_v<(int)1, (int)1, (int)2, (int)1>;
    constexpr auto case5 = tudb::is_same_value_types_v<(int)1, (int)1, (char)1, (int)1>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
    ASSERT_FALSE(case5);
}

TEST(tudbcpptest, CountTest)
{
    // 引数の数をカウント
    constexpr auto case1 = tudb::count_parameters_v<int>;
    constexpr auto case2 = tudb::count_parameters_v<int, void, double, long long>;
    constexpr auto case3 = tudb::count_value_parameters_v<1>;
    constexpr auto case4 = tudb::count_value_parameters_v<1, 'c', true, 4>;

    ASSERT_EQ(case1, 1);
    ASSERT_EQ(case2, 4);
    ASSERT_EQ(case3, 1);
    ASSERT_EQ(case4, 4);
}

TEST(tudbcpptest, GetDigitTest)
{
    constexpr auto case1 = tudb::get_digit<255>();
    constexpr auto case2 = tudb::get_digit<255, 2>();
    constexpr auto case3 = tudb::get_digit<255, 8>();
    constexpr auto case4 = tudb::get_digit<255, 16>();
    constexpr auto case5 = tudb::get_digit<-255>();
    constexpr auto case6 = tudb::get_digit<-255, 2>();
    constexpr auto case7 = tudb::get_digit<-255, 8>();
    constexpr auto case8 = tudb::get_digit<-255, 16>();
    constexpr auto case9 = tudb::get_digit<tudb::big_int<2>{~0ull, 80u}>();

    ASSERT_EQ(case1, 3);
    ASSERT_EQ(case2, 8);
    ASSERT_EQ(case3, 3);
    ASSERT_EQ(case4, 2);
    ASSERT_EQ(case5, 3);
    ASSERT_EQ(case6, 8);
    ASSERT_EQ(case7, 3);
    ASSERT_EQ(case8, 2);
    ASSERT_EQ(case9, 22);
}
