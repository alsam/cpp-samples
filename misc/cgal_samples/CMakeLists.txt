cmake_minimum_required(VERSION 3.7)

set(project_name cgal_polyhedron_volume_test)
project(${project_name})

find_package(GTest REQUIRED)
find_package(Boost REQUIRED)
find_package(CGAL REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CGAL_INCLUDE_DIRS})

add_executable(${project_name} cgal_polyhedron_volume_test.cpp)
target_link_libraries(${project_name} CGAL::CGAL GTest::GTest GTest::Main)

