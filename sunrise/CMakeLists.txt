cmake_minimum_required(VERSION 3.18)
set(project_name flag_for_sunrise)
project(${project_name})
set(project_lib ${project_name}_lib)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost REQUIRED COMPONENTS
  program_options
)

set(SUNRISE_PATH ${CMAKE_SOURCE_DIR}/3dparty/sunset)

set(SUNRISE_INCLUDE_DIRS ${SUNRISE_PATH}/src)

set(src
    ${SUNRISE_PATH}/src/sunset.cpp)

add_library(${project_lib} STATIC ${src})

add_executable(${project_name} src/sunrise_driver.cpp)

target_include_directories(${project_name} PRIVATE
                           ${SUNRISE_INCLUDE_DIRS}
                          )

target_link_libraries(${project_name}
    Boost::program_options
    ${project_lib}
)

