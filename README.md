# TuUtilCpp
C++20以降を対象とした、コンパイル時処理関連の汎用ヘッダーオンリーライブラリ。  

## 機能

|機能名|概要|名前空間|include|
|----|----|----|----|
|[mpl](https://github.com/TwilightUncle/TuUtilCpp/wiki/Mpl)|メタ関数群。<br>高階メタ関数によるパラメータパックの操作がメイン|`tuutil::mpl`|`#include<tuutilcpp/mpl.hpp>`|
|[cstr](#cstr--regex)|コンパイル時評価のための固定長文字列クラス|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
|[regex](#cstr--regex)|コンパイル時評価のための正規表現クラス。<br>ユニコード、制御文字については未対応。<br>特殊文字等は[ECMAScriptの正規表現](https://developer.mozilla.org/ja/docs/Web/JavaScript/Guide/Regular_Expressions)のふるまいを参考に作成|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
<!-- |db|コンパイル時に型検査、SQL生成等を行うO/Rマッパー。<br>全くできていない|`tuutil::db`|`#include<tuutilcpp/db.hpp>`| -->

※インクルードディレクティブの記載は、`include`ディレクトリをインクルードパスとして追加済みである前提とする。

## 環境
C++20以降の規格に対応したコンパイラを利用すること。  
  
以下、テスト実施環境
- Windows 11 Home, MSVC 19.34.31935.0, build type debug
- Windows 11 Home, MinGW64, GCC 12.2, build type debug
- Ubuntu 22.04(GitHub Actions), GCC 12.1, build type release
- Windows Server 2022(GitHub Actions), MSVC 19.34.31943.0, build type release

## Gitを用いたソースダウンロード例
```sh
cd path/to/dir
git clone https://github.com/TwilightUncle/TuUtilCpp.git TuUtilCpp
# path/to/dir/TuUtilCpp配下にソースが展開される
```

## 使用例
### mpl
```cpp
#include <tuple>
#include <type_traits>
#include <tuutilcpp/mpl.hpp>

using namespace tuutil;

// テンプレートパラメータを埋めていないメタ関数を型と同じ文脈で利用できるようにする
using quoted_is_same = mpl::quote<std::is_same>;
static_assert(!std::is_same_v<quoted_is_same, int>);  // 型としてメタ関数のテンプレート引数に指定できる

// テンプレート引数を部分適用することによって、quoted_is_sameからint型か判定するメタ関数を作成する
using binded_is_int = mpl::bind<quoted_is_same, int>;

// quote及び、bindしたメタ関数を実行する
static_assert(mpl::apply_v<quoted_is_same, int, int>); // std::is_same_v<int, int>と同様
static_assert(mpl::apply_v<binded_is_int, int>); // int型の場合のみ真となる

using types = std::tuple<int, double, float, char, int, float>;

// 整数型を抽出
using integral_types = mpl::extract_if_t<mpl::quote<std::is_integral>, types>;
static_assert(std::is_same_v<integral_types, std::tuple<int, char, int>>);

// int型を除去
using not_int_types = mpl::remove_if_t<mpl::bind<mpl::quote<std::is_same>, int>, types>;
static_assert(std::is_same_v<not_int_types, std::tuple<double, float, char, float>>);

// 重複している要素を除去
using unique_types = mpl::unique_t<types>;
static_assert(std::is_same_v<unique_types, std::tuple<int, double, float, char>>);

// 一部メタ関数は非型テンプレートパラメータパックを持つ型の要素操作にも対応している
template <auto... VParameters> struct val_list {};
using values = val_list<int(1), double(1), int(2), int(1), char(1), double(1), float(1)>;

// valuesに対して要素の並びの反転、要素の左回転、重複している要素(値と型どちらも一致しているもの)の削除を順に行う
using result_values = mpl::relay_t<
    values,
    mpl::type_list<
        mpl::quote<mpl::reverse>,
        mpl::quote<mpl::rotatel>,
        mpl::quote<mpl::unique>
    >
>;
static_assert(std::is_same_v<result_values, val_list<double(1), char(1), int(1), int(2), float(1)>>);
```
### cstr & regex
```cpp
#include <iostream>
#include <tuutilcpp/str.hpp>
using namespace tuutil;

// 非型テンプレート引数として日付文字列を受け取り、形式が不正な場合にコンパイルエラーを起こす
template <str::cstr DateStr>
struct DateType
{
    // 日付文字列についてパターンマッチを行い、ついでにキャプチャで年月日をそれぞれ抜き出す
    using date_pattern = str::regex<R"(^(\d{4})(?<delimiter>[/-])([01]\d)\k<delimiter>(\d{2})$)">;
    static constexpr auto match_result = date_pattern(DateStr);

    // パターンマッチに失敗したときコンパイルエラー
    static_assert(match_result.is_match(), "Specified 'DateStr' format is not 'yyyy[/-]mm[/-]dd'.");
    // キャプチャ等不要でパターンマッチだけしたい場合は以下でOK
    static_assert(date_pattern::match(DateStr));

    static constexpr auto year = str::to_int<int>(match_result[1]);
    static constexpr auto month = str::to_int<int>(match_result[3]);
    static constexpr auto day = str::to_int<int>(match_result[4]);

    // 月や日付のとりうる値の範囲から逸脱している時コンパイルエラー
    static_assert(month > 0 && month <= 12, "Possible months are between 1 and 12.");
    static_assert(day > 0 && day <= 31, "Possible dates are between 1 and 31.");
};

int main()
{
    using d_20200320 = DateType<"2020/03/20">;
    using d_20200321 = DateType<"2020-03-21">;

    // 使うとコンパイルエラー
    using error1 = DateType<"20222/03/20">;
    using error2 = DateType<"2020/03-20">;
    using error3 = DateType<"2020/13/20">;
    using error4 = DateType<"2020/00/20">;
    using error5 = DateType<"2020/03/32">;
    using error6 = DateType<"2020/03/00">;

    std::cout << d_20200320::year << "年" << d_20200320::month << "月" << d_20200320::day << "日" << std::endl;   // 2020年3月20日
    std::cout << d_20200321::year << "年" << d_20200321::month << "月" << d_20200321::day << "日" << std::endl;   // 2020年3月21日

    // static_assert failed: 'Specified 'DateStr' format is not 'yyyy[/-]mm[/-]dd'.' 
    // std::cout << error1::year << "年" << error1::month << "月" << error1::day << "日";
    // std::cout << error2::year << "年" << error2::month << "月" << error2::day << "日";

    // static_assert failed: 'Possible months are between 1 and 12.'
    // std::cout << error3::year << "年" << error3::month << "月" << error3::day << "日";
    // std::cout << error4::year << "年" << error4::month << "月" << error4::day << "日";

    // static_assert failed: 'Possible month are between 1 and 31.'
    // std::cout << error5::year << "年" << error5::month << "月" << error5::day << "日";
    // std::cout << error6::year << "年" << error6::month << "月" << error6::day << "日";
    return 0;
}
```

## テストのビルドコマンド
CMakeをインストールし、パスを通しておくこと。 
また、vcpkgもインストール済みであること。  
```sh
# 各自のソース展開先ディレクトリへ移動
cd path/to/dir/TuUtilCpp

# ビルド用ディレクトリ作成(初回のみ)/移動
mkdir build
cd build

# プロジェクトファイルの作成(初回のみ)
cmake -DTUUTILCPP_BUILD_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=path\to\vcpkg\scripts\buildsystems\vcpkg.cmake ..

# テストソースをビルド
cmake --build .

# テスト実施
ctest -C Debug
```