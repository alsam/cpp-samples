cmake_minimum_required(VERSION 3.9)
set(project_name poisson_solver)
set(project_name_tests ${project_name}_tests)
project(${project_name})

#========== Global Configurations =============#
#----------------------------------------------#

set(CMAKE_CXX_STANDARD 17)     
#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Eigen3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
find_package(pybind11 REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS
  program_options
  #  thread
  #  system
  #  regex
)

include_directories(
    ${PYTHON_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

#========== Targets Configurations ============#

add_executable(${project_name}
    src/cft.cpp
    src/PoissonProblem.cpp
    src/bs2.cpp
    src/main.cpp)

# Include directories list.
target_include_directories(${project_name} PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${project_name}
    Boost::program_options
)

add_executable(${project_name_tests} src/unit_tests.cpp src/cft.cpp)

target_link_libraries(${project_name_tests} GTest::GTest GTest::Main)
