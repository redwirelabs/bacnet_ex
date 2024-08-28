cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

# dependency manager
set(CPM_VERSION 0.40.2)
set(CPM_USE_NAMED_CACHE_DIRECTORIES false)
set(CPM_PATH "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_VERSION}.cmake")
if(NOT EXISTS ${CPM_PATH})
    message(STATUS "Downloading CPM.cmake to ${CPM_PATH}")
    set(CPM_GH_URL https://github.com/cpm-cmake/cpm.cmake/releases/download)
    set(CPM_RELEASE_URL ${CPM_GH_URL}/v${CPM_VERSION}/CPM.cmake)
    file(DOWNLOAD ${CPM_RELEASE_URL} ${CPM_PATH})
endif()
include(${CPM_PATH})

# dependencies
CPMFindPackage(
    NAME bacnet
    GITHUB_REPOSITORY bacnet-stack/bacnet-stack
    GIT_TAG bacnet-stack-1.3.8)

# setup erlang
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{ERL_EI_LIBDIR})
find_library(ERLANG_EI_LIB ei)
include_directories($ENV{ERL_EI_INCLUDE_DIR})

# sources
file(GLOB sources src/*.c)

# build
project(bacnetd)
set(CMAKE_BUILD_TYPE Release)
add_executable(bacnetd ${sources})
set_target_properties(bacnetd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE $ENV{MIX_APP_PATH}/priv)
target_link_libraries(bacnetd PRIVATE bacnet-stack::bacnet-stack ${ERLANG_EI_LIB})
