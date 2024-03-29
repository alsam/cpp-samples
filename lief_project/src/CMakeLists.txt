cmake_minimum_required(VERSION 3.0)

project(CMakeLIEF LANGUAGES CXX)

# Use LIEF with 'find_package()'
# ==============================

# Find LIEF. If LIEF was not installed into a default system directory then
# specify the following option during CMake configuration:
# -DLIEF_DIR=<LIEF install prefix>/share/LIEF/cmake
find_package(cxxopts REQUIRED)
find_package(LIEF REQUIRED COMPONENTS STATIC) # COMPONENTS: <SHARED | STATIC> - Default: STATIC

# Add our executable
# ==================
add_executable(HelloLIEF main.cpp)

# Enable C++17
set_property(TARGET HelloLIEF
             PROPERTY CXX_STANDARD           17
             PROPERTY CXX_STANDARD_REQUIRED  ON)

# Link the executable with LIEF
target_link_libraries(HelloLIEF PRIVATE LIEF::LIEF cxxopts::cxxopts)
