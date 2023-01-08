cmake_minimum_required(VERSION 3.9)
set(project_name poisson_solver)
set(project_lib ${project_name}_lib)
set(project_tests ${project_name}_tests)
project(${project_name})

#========== Global Configurations =============#
#----------------------------------------------#

set(CMAKE_CXX_STANDARD 17)     
#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Eigen3 REQUIRED)
find_package (Python3 COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS
  program_options
  #  thread
  #  system
  #  regex
)

#========== Targets Configurations ============#

set (src
     src/FastCosineTransform.cpp
     src/PoissonProblem.cpp
     src/BartelsStewart.cpp
     src/ChebyshevDifferentiate.cpp)

add_library(${project_lib} STATIC ${src})

target_link_libraries(${project_lib}
  PUBLIC
  Eigen3::Eigen
  pybind11::module
  Python3::Python
)

add_executable(${project_name} src/main.cpp)

target_link_libraries(${project_name}
    PUBLIC pybind11::headers
    Boost::program_options
    Eigen3::Eigen
    ${project_lib}
)

add_executable(${project_tests} src/unit_tests.cpp)

target_link_libraries(${project_tests}
                      GTest::GTest
                      GTest::Main
                      Eigen3::Eigen
                      ${project_lib}
)
