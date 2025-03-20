cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

# dependency manager
set(CPM_VERSION 0.40.2)
set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_SOURCE_DIR}/_build/cache/cpm)
set(CPM_USE_NAMED_CACHE_DIRECTORIES false)
set(CPM_PATH "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_VERSION}.cmake")
if(NOT EXISTS ${CPM_PATH})
    message(STATUS "Downloading CPM.cmake to ${CPM_PATH}")
    set(CPM_GH_URL https://github.com/cpm-cmake/cpm.cmake/releases/download)
    set(CPM_RELEASE_URL ${CPM_GH_URL}/v${CPM_VERSION}/CPM.cmake)
    file(DOWNLOAD ${CPM_RELEASE_URL} ${CPM_PATH})
endif()
include(${CPM_PATH})

# Don't need the patches for the current method
# file(GLOB PATCH_FILES "${CMAKE_SOURCE_DIR}/patches/*.patch")

# dependencies
#CPMFindPackage(
#    NAME bacnet
#    GITHUB_REPOSITORY bacnet-stack/bacnet-stack
#    GIT_TAG bacnet-stack-1.3.8
#    OPTIONS
#        "BACNET_PROTOCOL_REVISION 24"
#        "BACNET_STACK_BUILD_APPS NO"
#    PATCHES ${PATCH_FILES})

# setup karg
# set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_SOURCE_DIR}/karg/bacnet)

# old try - trying from our first idea "use karg/ directory"
# include_directories(karg/src/bacnet)
# file(GLOB_RECURSE KARG_SOURCES "karg/src/bacnet/*.c")
# file(GLOB_RECURSE KARG_HEADERS "karg/src/bacnet/*.h")

# setup erlang
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} $ENV{ERL_EI_LIBDIR})
find_library(ERLANG_EI_LIB ei)
include_directories($ENV{ERL_EI_INCLUDE_DIR})

# sources
INCLUDE_DIRECTORIES(src/)
set(SOURCES
    src/bacnet.c
    src/log.c
    src/main.c
    src/port.c
    src/protocol/decode_call.c
    src/protocol/enum.c)

# If not built by Mix, it is necessary to set the variable to
# the current directory
if(DEFINED ENV{MIX_APP_PATH} AND NOT "$ENV{MIX_APP_PATH}" STREQUAL "")
    set(MIX_APP_PATH $ENV{MIX_APP_PATH})
else()
    set(MIX_APP_PATH ".")
endif()

# build
project(bacnetd)
set(CMAKE_BUILD_TYPE Release)
add_executable(bacnetd ${SOURCES})
set_target_properties(bacnetd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MIX_APP_PATH}/priv)
target_link_libraries(bacnetd PRIVATE ${ERLANG_EI_LIB})
