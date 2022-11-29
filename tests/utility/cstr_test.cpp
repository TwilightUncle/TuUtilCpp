#include <gtest/gtest.h>
#include <utility.hpp>

TEST(tudbcpptest, CstrConstructorTest)
{
    constexpr auto case1 = tudb::cstr<5>{};
    constexpr auto case2 = tudb::cstr{"abcd"};
    // 最後尾のdは消えること(必ず配列の最後尾は終端文字である必要)
    constexpr auto case3 = tudb::cstr{{'a', 'b', 'c', 'd'}};

    // リテラルでない者の指定
    constexpr char str2[] = "abcd\0\0\0";
    constexpr auto case4 = tudb::cstr{str2};

    ASSERT_EQ(case1.max_size, 4);
    ASSERT_EQ(case1.size(), 0);
    ASSERT_STREQ(case1.data(), "");
    ASSERT_EQ(case2.max_size, 4);
    ASSERT_EQ(case2.size(), 4);
    ASSERT_STREQ(case2.data(), "abcd");
    ASSERT_EQ(case3.max_size, 3);
    ASSERT_EQ(case3.size(), 3);
    ASSERT_STREQ(case3.data(), "abc");
    ASSERT_EQ(case4.max_size, 7);
    ASSERT_EQ(case4.size(), 4);
    ASSERT_STREQ(case4.data(), "abcd");
}

TEST(tudbcpptest, CstrConcatTest)
{
    constexpr auto case1 = tudb::concat(tudb::cstr{"abc"}, tudb::cstr{"defghi"});
    constexpr auto case2 = tudb::concat(tudb::cstr{"abc"}, tudb::cstr{"1234"}, tudb::cstr{"zyxwv"});

    ASSERT_STREQ(case1.data(), "abcdefghi");
    ASSERT_STREQ(case2.data(), "abc1234zyxwv");
}

TEST(tudbcpptest, CstrSubstrTest)
{
    constexpr auto case1 = tudb::substr<3>(tudb::cstr{"abcdefg"});
    constexpr auto case2 = tudb::substr<3, 3>(tudb::cstr{"abcdefg"});
    constexpr auto case3 = tudb::substr<3, 4>(tudb::cstr{"abcdefg"});
    constexpr auto case4 = tudb::substr<3, 5>(tudb::cstr{"abcdefg"});

    ASSERT_STREQ(case1.data(), "defg");
    ASSERT_STREQ(case2.data(), "def");
    ASSERT_STREQ(case1.data(), case3.data());
    ASSERT_STREQ(case3.data(), case4.data());
}

TEST(tudbcpptest, CstrOperatorTest)
{
    constexpr auto case1 = tudb::cstr{"abc"} == tudb::cstr{"abc"};
    constexpr auto case2 = tudb::cstr{"abc"} == tudb::cstr{"adc"};
    constexpr auto case3 = tudb::cstr{"abc"} != tudb::cstr{"abcd"};
    constexpr auto case4 = tudb::cstr{"abc"} < tudb::cstr{"adc"};
    constexpr auto case5 = tudb::cstr{"abc"} >= tudb::cstr{"abcd"};

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
    ASSERT_FALSE(case5);
}

TEST(tudbcpptest, CstrIntegralToStringTest)
{
    constexpr auto case1 = tudb::to_string<(unsigned int)154352>();
    constexpr auto case2 = tudb::to_string<(long)-54362>();
    constexpr auto case3 = tudb::to_string<(unsigned char)0b100, 2>();
    constexpr auto case4 = tudb::to_string<(short)-0777, 8>();
    constexpr auto case5 = tudb::to_string<(unsigned long)0xffffffff, 16>(); 
    constexpr auto case6 = tudb::to_string<100, 10, true>(); 
    constexpr auto case7 = tudb::to_string<-0b1111, 2, true>(); 
    constexpr auto case8 = tudb::to_string<077777, 8, true>(); 
    constexpr auto case9 = tudb::to_string<-0xfffff, 16, true>();
    // constexprではステップ数の上限をコンパイラオプションで上げなければいけない(vc++の場合/constexpr:stepsを指定)
    constexpr auto case10 = tudb::to_string<tudb::big_int<2>{~0ull, 10u}>();

    ASSERT_STREQ(case1.data(), "154352");
    ASSERT_STREQ(case2.data(), "-54362");
    ASSERT_STREQ(case3.data(), "100");
    ASSERT_STREQ(case4.data(), "-777");
    ASSERT_STREQ(case5.data(), "FFFFFFFF");
    ASSERT_STREQ(case6.data(), "100");
    ASSERT_STREQ(case7.data(), "-0b1111");
    ASSERT_STREQ(case8.data(), "077777");
    ASSERT_STREQ(case9.data(), "-0xFFFFF");
    ASSERT_STREQ(case10.data(), "202914184810805067775");
}
