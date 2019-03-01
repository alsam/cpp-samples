cmake_minimum_required(VERSION 3.7)

set(project_name tiny_trimesh)
project(${project_name})

find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(${project_name} tests.cpp)
target_link_libraries(${project_name} GTest::GTest GTest::Main)

