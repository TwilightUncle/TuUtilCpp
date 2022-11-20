cmake_minimum_required(VERSION 3.22)
project(tudbcpp CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tudbcpp ${PROJECT_SOURCE_DIR}/main.cpp)
target_include_directories(tudbcpp PUBLIC ${PROJECT_SOURCE_DIR}/include)

# テストを追加していく
set(TUGAMECPP_TESTS
    ${PROJECT_SOURCE_DIR}/tests/utility/algorithm_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/utility/cstr_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/utility/carry_over_test.cpp
)
source_group(tests FILES ${TUGAMECPP_TESTS})

if (TUDBCPP_BUILD_TESTS)
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
    add_executable(tudbcpptest ${TUGAMECPP_TESTS})
    target_include_directories(tudbcpptest PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(tudbcpptest PRIVATE gtest_main)
    target_compile_features(tudbcpptest PRIVATE cxx_std_20)
    add_test(NAME tudbcpptest COMMAND tudbcpptest)

    # CTestへ単体テストを登録
    include(GoogleTest)
    gtest_discover_tests(tudbcpptest)
else (TUDBCPP_BUILD_TESTS)
    message(STATUS "TUDBCPP_BUILD_TESTS OFF")
endif (TUDBCPP_BUILD_TESTS)
