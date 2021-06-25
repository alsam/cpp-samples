cmake_minimum_required(VERSION 3.18)
set(project_name amgcl-block-matrices)
set(project_lib ${project_name}_lib)
set(project_unit_tests ${project_name}_unit_tests)
project(${project_name})

set(CMAKE_CXX_STANDARD 20)

find_package(amgcl)
get_property(AMGCL_INCLUDE_DIRS TARGET amgcl::amgcl PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

find_package(Boost REQUIRED COMPONENTS
  program_options
  #  thread
  #  system
  #  regex
)

add_executable(${project_name} src/amgcl_block_matrices_test.cpp)
add_compile_definitions(AMGCL_BLOCK_SIZES=\(2\)\(3\)\(6\)\(10\))
add_compile_definitions(SOLVER_BACKEND_CUDA=1)
target_link_libraries(${project_name}
    amgcl::amgcl
    Boost::program_options

    )

