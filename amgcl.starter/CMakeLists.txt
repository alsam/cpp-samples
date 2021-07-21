cmake_minimum_required(VERSION 3.16)
set(project_name amgcl_block_matrices)
#set(project_name solver)
set(project_lib ${project_name}_lib)
set(project_unit_tests ${project_name}_unit_tests)
project(${project_name})

set(CMAKE_CXX_STANDARD 17)

find_package(amgcl)

find_package(OpenMP)


#get_property(AMGCL_INCLUDE_DIRS TARGET amgcl::amgcl PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

set(AMGCL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3dparty/amgcl/)

#find_package(VexCL QUIET)
find_package(VexCL PATHS /home/alsam/local)
if (VexCL_FOUND)
    #message(STATUS "VexCL_FOUND")
    vexcl_add_executables(amgcl_block_matrices_test_vexcl src/amgcl_block_matrices_test.cpp)
    #target_compile_definitions(amgl_block_matrices_test_vexcl INTERFACE SOLVER_BACKEND_VEXCL)
    vexcl_add_executables(solver_vexcl src/solver.cpp)
    target_include_directories(amgcl_block_matrices_test_vexcl INTERFACE ${AMGCL_INCLUDE_DIRS} SOLVER_BACKEND_VEXCL)
    target_compile_definitions(solver_vexcl INTERFACE SOLVER_BACKEND_VEXCL)
    target_include_directories(solver_vexcl INTERFACE ${AMGCL_INCLUDE_DIRS} SOLVER_BACKEND_VEXCL)

endif()

find_package(CUDA)
if (CUDA_FOUND)
    set(CUDA_TARGET_ARCH "Maxwell Pascal Turing Ampere" CACHE STRING "Target architecture(s) for CUDA")
    cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS ${CUDA_TARGET_ARCH})

    if (OPENMP_FOUND)
        list(APPEND CUDA_NVCC_FLAGS -Xcompiler ${OpenMP_CXX_FLAGS})
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND CUDA_NVCC_FLAGS
            ${CUDA_ARCH_FLAGS} -std=c++17 -Wno-deprecated-gpu-targets)

        list(APPEND CUDA_NVCC_FLAGS -I${AMGCL_INCLUDE_DIRS} -Xcompiler -std=c++17 -Xcompiler -fPIC -Xcompiler -Wno-vla)
    endif()

    add_library(cuda_target INTERFACE)
    target_link_libraries(cuda_target INTERFACE ${CUDA_cusparse_LIBRARY})
endif()

find_package(Boost REQUIRED COMPONENTS
  program_options
  #  thread
  #  system
  #  regex
)

function(add_cuda_example example)
    configure_file(
        src/${example}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/${example}.cu
        COPYONLY
        )

    cuda_add_executable(${example}_cuda ${CMAKE_CURRENT_BINARY_DIR}/${example}.cu
        OPTIONS -DSOLVER_BACKEND_CUDA -I${CMAKE_CURRENT_SOURCE_DIR}/src -I/usr/include/eigen3 -Isrc/)
    target_compile_options(${example}_cuda INTERFACE ${OpenMP_CXX_FLAGS})
    target_link_libraries(${example}_cuda cuda_target ${OpenMP_CXX_FLAGS})
endfunction()

add_executable(amgl_block_matrices_test src/amgcl_block_matrices_test.cpp)

add_executable(solver src/solver.cpp)

add_cuda_example(amgcl_block_matrices_test)
add_cuda_example(solver)

add_compile_definitions(AMGCL_BLOCK_SIZES=\(2\)\(3\)\(6\)\(10\))
add_compile_definitions(REAL=${FLOAT_TYPE})

target_include_directories(amgl_block_matrices_test PRIVATE
src/
${AMGCL_INCLUDE_DIRS}
)

target_include_directories(solver PRIVATE
src/
${AMGCL_INCLUDE_DIRS}
)

target_link_libraries(amgl_block_matrices_test
    amgcl::amgcl
    cuda_target
    #    Boost::program_options
    )

target_link_libraries(solver
    amgcl::amgcl
    cuda_target
    #    Boost::program_options
    )

