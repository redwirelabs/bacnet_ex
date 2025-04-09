cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

# dependency manager
set(CPM_VERSION 0.40.5)
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

# target optimizations
if("$ENV{MIX_ENV}" STREQUAL "prod")
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

# output dir
if("$ENV{MIX_APP_PATH}" STREQUAL "")
    set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/_build/external)
else()
    set(OUTPUT_DIR $ENV{MIX_APP_PATH}/priv)
endif()

# dependencies
if(DEFINED CPM_bacnet_SOURCE)
    set(PATCHES "")
else()
    set(PATCHES
        patches/0001-Add-Model-and-Application-Software-Version-to-routed.patch
        patches/0002-Do-not-interpret-a-zero-d-out-device-in-a-static-lis.patch
        patches/0003-Remove-unnecessary-debug-logs-to-stderr.patch
        patches/0004-Add-configurable-vendor-name-support.patch
        patches/0005-Add-routed-analog-input-object-support.patch
        patches/0006-Add-routed-multistate-input-object-support.patch
        patches/0007-Allow-BACNET_PROTOCOL_REVISION-to-be-set-by-user.patch
        patches/0008-Exclude-object-identifier-from-the-common-prop-list.patch)
endif()

CPMFindPackage(
    NAME bacnet
    GITHUB_REPOSITORY bacnet-stack/bacnet-stack
    GIT_TAG bacnet-stack-1.3.8
    OPTIONS
        "BACNET_PROTOCOL_REVISION 24"
        "BACNET_STACK_BUILD_APPS NO"
    PATCHES ${PATCHES})

# setup erlang
find_library(libei NAMES ei HINTS $ENV{ERL_EI_LIBDIR} REQUIRED)

# sources
set(SOURCES
    src/bacnet.c
    src/log.c
    src/main.c
    src/port.c
    src/object/command.c
    src/protocol/decode_call.c
    src/protocol/enum.c
    src/protocol/event.c)

# build
project(bacnetd)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
add_executable(bacnetd ${SOURCES})

target_include_directories(bacnetd
    PRIVATE
        $ENV{ERL_EI_INCLUDE_DIR}
        src/)

target_link_libraries(bacnetd PRIVATE bacnet-stack ${libei})
