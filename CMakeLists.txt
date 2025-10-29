cmake_minimum_required(VERSION 3.25)

project(ShortMarch)

set(CMAKE_CXX_STANDARD 17)

set(LONGMARCH_DISABLE_PYTHON ON)

add_subdirectory(external/LongMarch)
add_subdirectory(src)