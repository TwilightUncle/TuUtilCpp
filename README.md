# TuUtilCpp
C++20以降を対象とした、コンパイル時処理関連の汎用ヘッダーオンリーライブラリ。  

## 機能

||概要|名前空間|include|
|----|----|----|----|
|mpl|俺俺メタ関数群。<br>高階メタ関数によるパラメータパックの操作がメイン|`tuutil::mpl`|`#include<tuutilcpp/mpl.hpp>`|
|cstr|コンパイル時評価のための固定長文字列クラス|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
|regex|コンパイル時評価のための正規表現クラス。<br>ユニコード、制御文字については未対応。<br>ECMAScriptの正規表現のふるまいを参考に作成|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
<!-- |db|コンパイル時に型検査、SQL生成等を行うO/Rマッパー。<br>全くできていない|`tuutil::db`|`#include<tuutilcpp/db.hpp>`| -->

※インクルードディレクティブの記載は、`include`ディレクトリをインクルードパスとして追加済みである前提とする。

## 環境
C++20以降の規格に対応したコンパイラを利用すること。  
Windows11において、Visual Studio 2022に付属のMSVCでビルド及びテスト実施。

## Gitを用いたソースダウンロード例
```sh
cd path/to/dir
git clone https://github.com/TwilightUncle/TuUtilCpp.git TuUtilCpp
# path/to/dir/TuUtilCpp配下にソースが展開される
```

## ソース例
### mpl
```cpp
#include <tuple>
#include <type_traits>
#include <tuutilcpp/mpl.hpp>
using namespace tuutil::mpl;

using types = std::tuple<int, double, float, char, int, float>;

// 整数型を抽出
using integral_types = extract_if_t<quote<std::is_integral>, types>;
static_assert(std::is_same_v<integral_types, std::tuple<int, char, int>>);

// int型を除去
using not_int_types = remove_if_t<bind<quote<std::is_same>, int>, types>;
static_assert(std::is_same_v<not_int_types, std::tuple<double, float, char, float>>);

// 重複している要素を除去
using unique_types = unique_t<types>;
static_assert(std::is_same_v<unique_types, std::tuple<int, double, float, char>>);

// 一部メタ関数は非型テンプレートパラメータパックを持つ型の要素操作にも対応している
template <auto... VParameters> struct val_list {};
using values = val_list<int(1), double(1), int(2), int(1), char(1), double(1), float(1)>;

// valuesに対して要素の並びの反転、要素の左回転、重複している要素(値と型どちらも一致しているもの)の削除を順に行う
using result_values = relay_t<
    values,
    type_list<
        quote<reverse>,
        quote<rotatel>,
        quote<unique>
    >
>;
static_assert(std::is_same_v<result_values, val_list<double(1), char(1), int(1), int(2), float(1)>>);
```

## テストのビルドコマンド
CMakeをインストールし、パスを通しておくこと。
```sh
# 各自のソース展開先ディレクトリへ移動
cd path/to/dir/TuUtilCpp

# ビルド用ディレクトリ作成(初回のみ)/移動
mkdir build
cd build

# プロジェクトファイルの作成(初回のみ)
cmake -DTUUTILCPP_BUILD_TESTS=ON ..

# テストソースをビルド
cmake --build .
```
ビルド後、以下のファイルが作成される(Windows環境でMSVCの場合)ので、そのままターミナルに打ち込んで実行することでテストが実施される。
- path\to\dir\TuUtilCpp\build\Debug\TuUtilCppTest.exe

```sh
# テスト実行
path\to\dir\TuUtilCpp\build\Debug\TuUtilCppTest.exe
```