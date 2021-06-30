cmake_minimum_required(VERSION 3.18)
#set(project_name amgcl_block_matrices)
set(project_name solver)
set(project_lib ${project_name}_lib)
set(project_unit_tests ${project_name}_unit_tests)
project(${project_name})

set(CMAKE_CXX_STANDARD 20)

find_package(amgcl)

find_package(OpenMP)

#find_package(VexCL QUIET)
if (VexCL_FOUND)
    #message(STATUS "VexCL_FOUND")
    vexcl_add_executables(${example}_vexcl src/amgcl_block_matrices_test.cpp)
    target_compile_definitions(${solver}_vexcl INTERFACE SOLVER_BACKEND_VEXCL)

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

        list(APPEND CUDA_NVCC_FLAGS -Xcompiler -std=c++17 -Xcompiler -fPIC -Xcompiler -Wno-vla)
    endif()

    add_library(cuda_target INTERFACE)
    target_link_libraries(cuda_target INTERFACE ${CUDA_cusparse_LIBRARY})
endif()


get_property(AMGCL_INCLUDE_DIRS TARGET amgcl::amgcl PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

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

#add_executable(${project_name} src/amgcl_block_matrices_test.cpp)
add_executable(${project_name} src/${project_name}.cpp)

add_cuda_example(amgcl_block_matrices_test)
add_cuda_example(${project_name})

add_compile_definitions(AMGCL_BLOCK_SIZES=\(2\)\(3\)\(6\)\(10\))
target_include_directories(${project_name} PRIVATE
src/
)
target_link_libraries(${project_name}
    amgcl::amgcl
    cuda_target
    #    Boost::program_options
    )

