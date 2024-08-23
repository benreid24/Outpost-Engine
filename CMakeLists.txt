cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(ProjectName C CXX)

# BLIB will inherit these settings as well
option(STATIC_LINK "On to link statically" On)
option(BUILD_TESTS "On to build unit tests" On)
option(BUILD_EXAMPLES "On to build examples" On)
option(ON_CI "True when on CI, false when local" Off)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_INSTALL_PREFIX .)

if (ON_CI)
    add_compile_definitions(ON_CI)
endif()

add_subdirectory(lib/BLIB)

include("lib/BLIB/cmake/set_module_path.cmake")
set_module_path()

include(configure_blib_target)
include(create_blib_target)
include(link_blib_target)

add_library(Core STATIC)
configure_blib_target(Core)
link_blib_target(Core)

create_blib_target(
    TARGET ProjectName
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    LINK_LIBRARIES Core
)

create_blib_target(
    TARGET ProjectNameEditor
    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    LINK_LIBRARIES Core
)

add_executable(Bundler)
configure_blib_target(Bundler)

include_directories(include)
add_subdirectory(src)
add_subdirectory(include)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
