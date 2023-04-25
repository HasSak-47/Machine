cmake_minimum_required(VERSION 2.6)
project(vm)

file(GLOB_RECURSE compile_units src/*.c src/*.cpp)
include_directories(include)
add_executable(vm ${compile_units})
