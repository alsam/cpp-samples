cmake_minimum_required(VERSION 3.18)
project(amgcl-block-matrices)

find_package(amgcl)
get_property(AMGCL_INCLUDE_DIRS TARGET amgcl::amgcl PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

find_package(Boost)

add_executable(block_matrices_test_amgcl src/amgcl_block_matrices_test.cpp)
target_link_libraries(block_matrices_test_amgcl
    amgcl::amgcl
    )

