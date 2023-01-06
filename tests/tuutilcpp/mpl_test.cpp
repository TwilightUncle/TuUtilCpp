#include <gtest/gtest.h>
#include <tuutilcpp/mpl.hpp>

using namespace tuutil::mpl;

template <class... Args>
struct dummy_metafunction
{
    using type = std::true_type;
    static constexpr auto value = true;
};

template <class... Args>
struct dummy_non_metafunction {};

TEST(tuutilcpp_mpl_test, MetaCallbackTest)
{
    constexpr auto case1 = ReturenValueMetaFunction<dummy_metafunction<int, float>>;
    constexpr auto case2 = ReturenValueMetaFunction<int>;
    constexpr auto case3 = ReturenValueMetaFunction<std::type_identity<int>>;
    constexpr auto case4 = ReturenValueMetaFunction<dummy_non_metafunction<int, float>>;
    constexpr auto case5 = ReturenTypeMetaFunction<dummy_metafunction<int, float>>;
    constexpr auto case6 = ReturenTypeMetaFunction<int>;
    constexpr auto case7 = ReturenTypeMetaFunction<std::type_identity<int>>;
    constexpr auto case8 = ReturenTypeMetaFunction<dummy_non_metafunction<int, float>>;

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_FALSE(case3);
    ASSERT_FALSE(case4);
    ASSERT_TRUE(case5);
    ASSERT_FALSE(case6);
    ASSERT_TRUE(case7);
    ASSERT_FALSE(case8);

    using type1 = quote<dummy_metafunction>;        // quoteされたメタ関数
    using type2 = bind<type1, int, float>;          // 部分適用
    using type3 = bind<type2, void, bool>;          // bindのネスト
    using type4 = quote<dummy_non_metafunction>;
    using type5 = bind<type4, int, float>;
    using type6 = bind<type5, void, bool>;

    constexpr auto case9 = MetaCallable<type1>;
    constexpr auto case10 = MetaCallable<type2>;
    constexpr auto case11 = MetaCallable<type3>;
    constexpr auto case12 = MetaCallable<type4>;
    constexpr auto case13 = MetaCallable<type5>;
    constexpr auto case14 = MetaCallable<type6>;
    constexpr auto case15 = MetaCallable<dummy_metafunction<int, float>>;
    constexpr auto case16 = MetaCallable<dummy_non_metafunction<int, float>>;

    ASSERT_TRUE(case9);
    ASSERT_TRUE(case10);
    ASSERT_TRUE(case11);
    ASSERT_TRUE(case12);
    ASSERT_TRUE(case13);
    ASSERT_TRUE(case14);
    ASSERT_FALSE(case15);
    ASSERT_FALSE(case16);

    constexpr auto case17 = std::is_same_v<apply_t<type1, int, float>, std::true_type>;
    constexpr auto case18 = std::is_same_v<apply_t<type2, int, float>, std::true_type>;
    constexpr auto case19 = std::is_same_v<apply_t<type3, int, float>, std::true_type>;
    // 以下「Invalid argment. Non-metafunction specified for argument F.」と出力されコンパイルエラー
    // constexpr auto case20 = std::is_same_v<apply_t<type4, int, float>, std::true_type>;
    // constexpr auto case21 = std::is_same_v<apply_t<type5, int, float>, std::true_type>;
    // constexpr auto case22 = std::is_same_v<apply_t<type6, int, float>, std::true_type>;
    constexpr auto case23 = apply_v<type1, int, float>;
    constexpr auto case24 = apply_v<type2, int, float>;
    constexpr auto case25 = apply_v<type3, int, float>;
    // ReturenValueMetaFunction制約に引っ掛かりエラー
    // constexpr auto case26 = apply_v<quote<std::type_identity>, int>;
    // 以下「Invalid argment. Non-metafunction specified for argument F.」と出力されコンパイルエラー
    // constexpr auto case27 = apply_v<type4, int, float>;
    // constexpr auto case28 = apply_v<type5, int, float>;
    // constexpr auto case29 = apply_v<type6, int, float>;

    ASSERT_TRUE(case17);
    ASSERT_TRUE(case18);
    ASSERT_TRUE(case19);
    ASSERT_TRUE(case23);
    ASSERT_TRUE(case24);
    ASSERT_TRUE(case25);

    // apply_withはremove_ifの実装に利用しているため、そちらのテストで問題なければOK
}

TEST(tuutilcpp_mpl_test, MapTest)
{
    using type_list = dummy_non_metafunction<long long, short, unsigned short, unsigned long long, unsigned long>;
    using expect_type_list = dummy_non_metafunction<const long long, const short, const unsigned short, const unsigned long long, const unsigned long>;
    using maped_type_list = map_t<quote<std::add_const>, type_list>;

    constexpr auto case1 = std::is_same_v<maped_type_list, expect_type_list>;
    EXPECT_TRUE(case1);
}

template <class T1, class T2> struct get_max_size_type : public std::conditional<(sizeof(T1) > sizeof(T2)), T1, T2> {};
template <class T1, class T2> struct get_min_size_type : public std::conditional<(sizeof(T1) <= sizeof(T2)), T1, T2> {};

TEST(tuutilcpp_mpl_test, FoldTest)
{
    using type_list = dummy_non_metafunction<long long, short, unsigned short, unsigned long long, unsigned long>;
    using l_max_type = foldl_t<quote<get_max_size_type>, long, type_list>;
    using l_min_type = foldl_t<quote<get_min_size_type>, long, type_list>;

    constexpr auto case1 = std::is_same_v<l_max_type, unsigned long long>;
    constexpr auto case2 = std::is_same_v<l_min_type, short>;
    
    EXPECT_TRUE(case1);
    EXPECT_TRUE(case2);

    using r_max_type = foldr_t<quote<get_max_size_type>, long, type_list>;
    using r_min_type = foldr_t<quote<get_min_size_type>, long, type_list>;

    constexpr auto case3 = std::is_same_v<r_max_type, unsigned long long>;
    constexpr auto case4 = std::is_same_v<r_min_type, short>;
    
    EXPECT_TRUE(case3);
    EXPECT_TRUE(case4);
}

TEST(tuutilcpp_mpl_test, PushTest)
{
    using type_list = dummy_non_metafunction<int, float>;

    constexpr auto case1 = std::is_same_v<push_back_t<type_list, void>, dummy_non_metafunction<int, float, void>>;
    constexpr auto case2 = std::is_same_v<push_front_t<type_list, void>, dummy_non_metafunction<void, int, float>>;
    constexpr auto case3 = std::is_same_v<push_back_if_t<quote<std::is_integral>, type_list, char>, dummy_non_metafunction<int, float, char>>;
    constexpr auto case4 = std::is_same_v<push_back_if_t<quote<std::is_integral>, type_list, void>, type_list>;
    constexpr auto case5 = std::is_same_v<push_front_if_t<quote<std::is_integral>, type_list, char>, dummy_non_metafunction<char, int, float>>;
    constexpr auto case6 = std::is_same_v<push_front_if_t<quote<std::is_integral>, type_list, void>, type_list>;

    EXPECT_TRUE(case1);
    EXPECT_TRUE(case2);
    EXPECT_TRUE(case3);
    EXPECT_TRUE(case4);
    EXPECT_TRUE(case5);
    EXPECT_TRUE(case6);
}

TEST(tuutilcpp_mpl_test, FindIfTest)
{
    using type_list = dummy_non_metafunction<double, long long, char, int>;
    using find_type_int = find_if_t<bind<quote<std::is_same>, int>, type_list>;
    using not_find_type = find_if_t<bind<quote<std::is_same>, short>, type_list>;

    constexpr auto case1 = std::is_same_v<find_type_int, int>;
    constexpr auto case2 = std::is_same_v<not_find_type, ignore_type>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
}

TEST(tuutilcpp_mpl_test, ExtractIfTest)
{
    using type_list = dummy_non_metafunction<void, std::string, int, unsigned char, short, std::nullptr_t, long long>;
    using extracted_integral_list = extract_if_t<quote<std::is_integral>, type_list>;
    using not_extracted_type = extract_if_t<quote<std::is_floating_point>, type_list>;
    using removed_integral_list = remove_if_t<quote<std::is_integral>, type_list>;
    using not_removed_list = remove_if_t<quote<std::is_floating_point>, type_list>;

    constexpr auto case1 = std::is_same_v<extracted_integral_list, dummy_non_metafunction<int, unsigned char, short, long long>>;
    constexpr auto case2 = std::is_same_v<not_extracted_type, ignore_type>;
    constexpr auto case3 = std::is_same_v<removed_integral_list, dummy_non_metafunction<void, std::string, std::nullptr_t>>;
    constexpr auto case4 = std::is_same_v<not_removed_list, type_list>;

    ASSERT_TRUE(case1);
    ASSERT_TRUE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
}
