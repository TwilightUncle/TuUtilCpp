#include <gtest/gtest.h>
#include <tustrcpp/cstr.hpp>

using namespace std::string_view_literals;

TEST(tustrcpptest, CstrConstructorTest)
{
    constexpr auto case1 = tustr::cstr<5>{};
    constexpr auto case2 = tustr::cstr{"abcd"};
    // 最後尾のdは消えること(必ず配列の最後尾は終端文字である必要)
    constexpr auto case3 = tustr::cstr{{'a', 'b', 'c', 'd'}};

    // リテラルでない者の指定
    constexpr char str2[] = "abcd\0\0\0";
    constexpr auto case4 = tustr::cstr{str2};

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

TEST(tustrcpptest, CstrConcatTest)
{
    constexpr auto case1 = tustr::concat(tustr::cstr{"abc"}, tustr::cstr{"defghi"});
    constexpr auto case2 = tustr::concat(tustr::cstr{"abc"}, tustr::cstr{"1234"}, tustr::cstr{"zyxwv"});

    ASSERT_STREQ(case1.data(), "abcdefghi");
    ASSERT_STREQ(case2.data(), "abc1234zyxwv");
}

TEST(tustrcpptest, CstrSubstrTest)
{
    constexpr auto case1 = tustr::substr<3>(tustr::cstr{"abcdefg"});
    constexpr auto case2 = tustr::substr<3, 3>(tustr::cstr{"abcdefg"});
    constexpr auto case3 = tustr::substr<3, 4>(tustr::cstr{"abcdefg"});
    constexpr auto case4 = tustr::substr<3, 5>(tustr::cstr{"abcdefg"});

    ASSERT_STREQ(case1.data(), "defg");
    ASSERT_STREQ(case2.data(), "def");
    ASSERT_STREQ(case1.data(), case3.data());
    ASSERT_STREQ(case3.data(), case4.data());
}

TEST(tustrcpptest, CstrOperatorTest)
{
    constexpr auto case1 = tustr::cstr{"abc"} == tustr::cstr{"abc"};
    constexpr auto case2 = tustr::cstr{"abc"} == tustr::cstr{"adc"};
    constexpr auto case3 = tustr::cstr{"abc"} != tustr::cstr{"abcd"};
    constexpr auto case4 = tustr::cstr{"abc"} < tustr::cstr{"adc"};
    constexpr auto case5 = tustr::cstr{"abc"} >= tustr::cstr{"abcd"};

    ASSERT_TRUE(case1);
    ASSERT_FALSE(case2);
    ASSERT_TRUE(case3);
    ASSERT_TRUE(case4);
    ASSERT_FALSE(case5);
}

TEST(tustrcpptest, CstrIntegralToStringTest)
{
    constexpr auto case1 = tustr::to_string<(unsigned int)154352>();
    constexpr auto case2 = tustr::to_string<(long)-54362>();
    constexpr auto case3 = tustr::to_string<(unsigned char)0b100, 2>();
    constexpr auto case4 = tustr::to_string<(short)-0777, 8>();
    constexpr auto case5 = tustr::to_string<(unsigned long)0xffffffff, 16>(); 
    constexpr auto case6 = tustr::to_string<100, 10, true>(); 
    constexpr auto case7 = tustr::to_string<-0b1111, 2, true>(); 
    constexpr auto case8 = tustr::to_string<077777, 8, true>(); 
    constexpr auto case9 = tustr::to_string<-0xfffff, 16, true>();
    // constexprではステップ数の上限をコンパイラオプションで上げなければいけない(vc++の場合/constexpr:stepsを指定)
    constexpr auto case10 = tustr::to_string<tudb::big_int<2>{~0ull, 10u}>();

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

TEST(tustrcpptest, CstrDevideByDelimiterTest)
{
    constexpr auto inst1 = tustr::cstr{"abc, abcd, abc,abcde, a"};

    EXPECT_EQ(inst1.count(", "), 3);
    EXPECT_EQ(inst1.count("abcd"), 2);
    EXPECT_EQ(inst1.count("abcdef"), 0);
    EXPECT_EQ(inst1.count("a"), 5);
    EXPECT_EQ(inst1.find(", "), 3);
    EXPECT_EQ(inst1.find("abcd", 6), 15);
    EXPECT_EQ(inst1.find("abcdef"), std::string_view::npos);
    EXPECT_EQ(inst1.find("a"), 0);

    constexpr auto case1 = tustr::split<"abc, abcd, abc,abcde, a", ", ">();
    ASSERT_EQ(case1.size(), 4);
    EXPECT_EQ(case1[0], "abc"sv);
    EXPECT_EQ(case1[1], "abcd"sv);
    EXPECT_EQ(case1[2], "abc,abcde"sv);
    EXPECT_EQ(case1[3], "a"sv);

    constexpr auto case2 = tustr::split<"abc, abcd, abc,abcde, a", "a">();
    ASSERT_EQ(case2.size(), 6);
    EXPECT_EQ(case2[0], ""sv);
    EXPECT_EQ(case2[1], "bc, "sv);
    EXPECT_EQ(case2[2], "bcd, "sv);
    EXPECT_EQ(case2[3], "bc,"sv);
    EXPECT_EQ(case2[4], "bcde, "sv);
    EXPECT_EQ(case2[5], ""sv);
}
