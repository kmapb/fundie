cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
   fundie
   VERSION 1.0
   LANGUAGES CXX)

add_library(fund_logic stage.cpp round.cpp position.cpp fund.cpp)
add_executable(fundie main.cpp)
target_link_libraries(fundie fund_logic pthread)

# More GTest palaver
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

function(add_gtest_executable exec_name)
add_executable(${exec_name} ${ARGN})
target_link_libraries(${exec_name} ${GTEST_LIBRARIES} GTest::Main fund_logic )
add_test(${exec_name} ${exec_name})
endfunction()

add_gtest_executable(test_position test/test_position.cpp)
add_gtest_executable(test_stage test/test_stage.cpp)
add_gtest_executable(test_fund test/test_fund.cpp)
