cmake_minimum_required(VERSION 3.12)
project(set)

set(CMAKE_CXX_STANDARD 17)

add_executable(set_test gtest/gtest.h gtest/gtest-all.cc gtest/gtest_main.cc
        set.h counted.cpp counted.h fault_injection.cpp fault_injection.h set_testing.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

add_executable(main main.cpp)