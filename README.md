# TuDbCpp
主要な機能はまだできていない。  
コンパイル時にデータベースの型チェック等を可能とするO/Rマッピングライブラリの作成を目指す。  
また、SQLクエリも可能な限りコンパイル時に生成済みであることを目的とする。  
現状、Header Onlyのため、本ライブラリを配置したあと、インクルードディレクトリにincludeフォルダを追加してやれば使用可能であり、別途ライブラリのビルドは不要。　　

## 環境
C++20以降の規格に対応したコンパイラを利用すること。  
テストはWindows11において、Visual Studio 2022に付属のMSVCでビルドしており、他環境やコンパイラでの確認は行っていないため、必要に応じて各環境でビルド及びテストを実施し、各自使用可能であるか確認する必要がある。

## Gitを用いたソースダウンロード例
```sh
cd path/to/dir
git clone https://github.com/TwilightUncle/TuDbCpp.git TuDbCpp
# path/to/dir/TuDbCpp配下にソースが展開される
```

## テストのビルドコマンド
CMakeを使用するため、最新のものをインストールし、パスを通しておくこと。
```sh
# 各自のソース展開先ディレクトリへ移動
cd path/to/dir/TuDbCpp

# ビルド用ディレクトリ作成(初回のみ)/移動
mkdir build
cd build

# プロジェクトファイルの作成(初回のみ)
cmake -DTUDBCPP_BUILD_TESTS=ON ..

# テストソースをビルド
cmake --build .
```
ビルド後、以下のファイルが作成される(Windows環境でMSVCの場合)ので、そのままターミナルに打ち込んで実行することでテストが実施される。
- path\to\dir\TuDbCpp\build\Debug\tudbcpptest.exe

```sh
# テスト実行
path\to\dir\TuDbCpp\build\Debug\tudbcpptest.exe
```