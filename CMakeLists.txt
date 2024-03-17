cmake_minimum_required(VERSION 3.10)
project(jj VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(jj ${SOURCES})
