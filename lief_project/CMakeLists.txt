cmake_minimum_required(VERSION 3.0)

project(CMakeLIEF LANGUAGES NONE)

include(ExternalProject)

# LIEF integration as an External Project
# ===========================
set(LIEF_PREFIX       "${CMAKE_CURRENT_BINARY_DIR}/LIEF")
set(LIEF_INSTALL_DIR  "${LIEF_PREFIX}/install")

# URL of the LIEF repo (Can be your fork)
set(LIEF_GIT_URL "https://github.com/lief-project/LIEF.git")

# LIEF's version to be used (can be 'master')
#set(LIEF_VERSION 0.12.0)
set(LIEF_VERSION master)

# LIEF compilation config
set(LIEF_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
  -DLIEF_DOC=OFF
  -DLIEF_PYTHON_API=OFF
  -DLIEF_EXAMPLES=OFF
  -DLIEF_TESTS=OFF
)

# cxxopts integration as an External Project This is a header only library
# ===========================
set(CXXOPTS_PREFIX       "${CMAKE_CURRENT_BINARY_DIR}/cxxopts")
set(CXXOPTS_INSTALL_DIR  "${CXXOPTS_PREFIX}/install")
set(CXXOPTS_GIT_URL "https://github.com/jarro2783/cxxopts.git")
set(CXXOPTS_VERSION master)
set(CXXOPTS_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCXXOPTS_ENABLE_INSTALL=ON
  -DCXXOPTS_BUILD_EXAMPLES=OFF
  -DCXXOPTS_BUILD_TESTS=OFF
)

if(MSVC)
  list(APPEND ${LIEF_CMAKE_ARGS} -DLIEF_USE_CRT_RELEASE=MT)
endif()

ExternalProject_Add(LIEF
  PREFIX           "${LIEF_PREFIX}"
  GIT_REPOSITORY   "${LIEF_GIT_URL}"
  GIT_TAG          ${LIEF_VERSION}
  # You may specify an existing LIEF source directory if you don't want to
  # download. Just comment out the above ``GIT_*`` commands and uncoment the
  # following ``SOURCE_DIR`` line
  #SOURCE_DIR       "${CMAKE_CURRENT_LIST_DIR}/../../.."
  INSTALL_DIR      "${LIEF_INSTALL_DIR}"
  CMAKE_ARGS       ${LIEF_CMAKE_ARGS}
)

ExternalProject_Add(CXXOPTS
  PREFIX           "${CXXOPTS_PREFIX}"
  GIT_REPOSITORY   "${CXXOPTS_GIT_URL}"
  GIT_TAG          ${CXXOPTS_VERSION}
  INSTALL_DIR      "${CXXOPTS_INSTALL_DIR}"
  CMAKE_ARGS       ${CXXOPTS_CMAKE_ARGS}
)

# User project
# ============
ExternalProject_Add(HelloLIEF
  DEPENDS         LIEF
  SOURCE_DIR      "${CMAKE_CURRENT_LIST_DIR}/src"
  BINARY_DIR      "${CMAKE_CURRENT_BUILD_DIR}"
  INSTALL_COMMAND ""
  CMAKE_ARGS
    "-DLIEF_DIR=${LIEF_INSTALL_DIR}/share/LIEF/cmake"
    "-Dcxxopts_DIR=${CXXOPTS_INSTALL_DIR}/lib/cmake/cxxopts"
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
)
