# TuUtilCpp
C++20以降を対象とした、コンパイル時処理関連の汎用ヘッダーオンリーライブラリ。  

## 機能

||概要|名前空間|include|
|----|----|----|----|
|mpl|俺俺メタ関数群|`tuutil::mpl`|`#include<tuutilcpp/mpl.hpp>`|
|cstr|コンパイル時評価のための固定長文字列クラス|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
|regex|コンパイル時評価のための正規表現クラス。<br>ユニコード、制御文字については未対応。<br>ECMAScriptの正規表現のふるまいを参考に作成|`tuutil::str`|`#include<tuutilcpp/str.hpp>`|
|db|コンパイル時に型検査、SQL生成等を行うO/Rマッパー。<br>全くできていない|`tuutil::db`|`#include<tuutilcpp/db.hpp>`|

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