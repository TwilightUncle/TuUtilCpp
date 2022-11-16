cmake_minimum_required(VERSION 3.22)
project(c20lab CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tudbcpp ${PROJECT_SOURCE_DIR}/main.cpp)
target_include_directories(tudbcpp PUBLIC ${PROJECT_SOURCE_DIR}/include)
