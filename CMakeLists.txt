cmake_minimum_required(VERSION 3.22)
project(TuUtilCpp CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(TuUtilCpp ${PROJECT_SOURCE_DIR}/main.cpp)
target_include_directories(TuUtilCpp PUBLIC ${PROJECT_SOURCE_DIR}/include)

# テストを追加していく
set(TUUTILCPP_TESTS
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/mpl_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/algorithm_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/carry_over_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/big_int_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/cstr_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/regex_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/column_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/db_definition_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/tuutilcpp/db_test.cpp
)
source_group(tests FILES ${TUUTILCPP_TESTS})

if (TUUTILCPP_BUILD_TESTS)
    # GoogleTestをダウンロード
    include(FetchContent)
        FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
    )
    # Google Testの不要なキャッシュ変数をオフにしておく
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    # 親プロジェクトのコンパイラ・リンカ設定を上書きするのを防ぐ（Windowsのみ）
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    # googletestをターゲットとして使えるようにする
    FetchContent_MakeAvailable(googletest)

    #単体テスト定義
    enable_testing()

    # 単体テストの定義
    add_executable(TuUtilCppTest ${TUUTILCPP_TESTS})
    # big_intの定数式評価ステップ数がデフォルトでは足りない(vc++ではそうであった)
    target_compile_options(TuUtilCppTest PRIVATE $<$<CXX_COMPILER_ID:MSVC>: /constexpr:steps1500000>)
    target_include_directories(TuUtilCppTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(TuUtilCppTest PRIVATE gtest_main)
    target_compile_features(TuUtilCppTest PRIVATE cxx_std_20)
    add_test(NAME TuUtilCppTest COMMAND TuUtilCppTest)

    # CTestへ単体テストを登録
    include(GoogleTest)
    gtest_discover_tests(TuUtilCppTest)
else (TUUTILCPP_BUILD_TESTS)
    message(STATUS "TUUTILCPP_BUILD_TESTS OFF")
endif (TUUTILCPP_BUILD_TESTS)
