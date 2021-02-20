cmake_minimum_required(VERSION 3.9)
project(poisson_solver)

#========== Global Configurations =============#
#----------------------------------------------#

set(CMAKE_CXX_STANDARD 17)     
#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Eigen3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)
find_package(pybind11 REQUIRED)

#========== Targets Configurations ============#

add_executable(poisolve
    src/cft.cpp
    src/PoissonProblem.cpp
    src/main.cpp
)

# Include directories list.
target_include_directories(poisolve PRIVATE 
    ${PYTHON_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)
