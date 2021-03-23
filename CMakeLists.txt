cmake_minimum_required(VERSION 3.17)

project(miniProject)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)

set(HEADERS objects.h)
set(CODE objects.cpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(miniProject t.cpp  ${HEADERS} ${CODE})
target_link_libraries(miniProject GL SDL2)
