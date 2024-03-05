cmake_minimum_required(VERSION 3.1...3.28)
set(CMAKE_CXX_STANDARD 17)

project(
   fundie
   VERSION 1.0
   LANGUAGES CXX)

add_executable(fundie main.cpp stage.cpp)
